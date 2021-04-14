#pragma once
#include <utility>

// just a simple singly-linked-list type wrapper
template <typename T>
struct HashmapBucketElem {
	// order doesn't really matter as we get 8-byte alignment with the pointer anyways
	HashmapBucketElem<T>* next;
	T super;

	template <typename ... Args>
	HashmapBucketElem(HashmapBucketElem<T>* _next, Args&& ... args)
		: next(_next), super(std::forward<Args>(args)...) {}
};

template <typename T>
struct HashmapBucket {
	HashmapBucketElem<T>* first = nullptr;

	~HashmapBucket() {
		HashmapBucketElem<T>* cur = first;
		while (cur != nullptr) {
			HashmapBucketElem<T>* next = cur->next;
			delete cur;
			cur = next;
		}
	}

	// TODO maybe adding an unused first parameter might improve performance by not
	// having to move arguments around, as both our caller (Hashmap::insert) and
	// HashmapBucketElem::HashmapBucketElem take one more parameter before the variadic
	// stuff
	template <typename ... Args>
	void insert(Args&& ... args) {
		HashmapBucketElem<T>* expected_first = __atomic_load_n(&first, __ATOMIC_SEQ_CST);
		HashmapBucketElem<T>* next =
			new HashmapBucketElem<T>(expected_first, std::forward<Args>(args)...);

		// try to replace first with next,
		// but only if next->next is still pointing to the current first
		while (__atomic_compare_exchange_n(&first, &expected_first, next,
			false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST) == false)
		{
			expected_first = __atomic_load_n(&first, __ATOMIC_SEQ_CST);
			next->next = expected_first;
		}
	}

	// linear search. returns nullptr if not found
	template <typename U>
	T* get(const U& callback) {
		HashmapBucketElem<T>* cur = __atomic_load_n(&first, __ATOMIC_SEQ_CST);

		while (cur != nullptr && !callback(&cur->super))
			cur = cur->next;

		if (cur == nullptr)
			return nullptr;
		else
			return &cur->super;
	}

#ifndef NDEBUG
	int num_elems() {
		int ret = 0;
		HashmapBucketElem<T>* cur = __atomic_load_n(&first, __ATOMIC_SEQ_CST);
		while (cur != nullptr) {
			cur = cur->next;
			ret++;
		}
		return ret;
	}
#endif
};

template <typename T>
struct AtomicHashmap {
	HashmapBucket<T>* table;
	int bits;

#ifndef NDEBUG
	size_t elems_used = 0;
#endif

	AtomicHashmap(int _bits) : bits(_bits) {
		size_t elems = (size_t)1 << bits;
		table = new HashmapBucket<T>[elems];
	}

	~AtomicHashmap() {
#ifndef NDEBUG
		printf("Usage of hashmap just before destruction: %zu/%zu\n",
				elems_used, (size_t)1 << bits);

		int max_collisions = 0;
		for (size_t i = 0; i < ((size_t)1 << bits); i++) {
			max_collisions = std::max(max_collisions, table[i].num_elems());
		}
		printf("Maximum number of collisions: %d\n", max_collisions);
#endif

		delete[] table;
	}

	template <typename ... Args>
	void insert(uint64_t hash, Args&& ... args) {
		hash >>= 64 - bits;
		table[hash].insert(std::forward<Args>(args)...);

#ifndef NDEBUG
		elems_used++;
#endif
	}

	template <typename U>
	T* get(uint64_t hash, U&& callback) {
		hash >>= 64 - bits;
		return table[hash].get(std::forward<U>(callback));
	}
};
