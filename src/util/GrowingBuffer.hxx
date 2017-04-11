/*
 * Copyright (C) 2016 Max Kellermann <max.kellermann@gmail.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * FOUNDATION OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GROWING_BUFFER_HXX
#define GROWING_BUFFER_HXX

#include "AllocatedArray.hxx"
#include "ConstBuffer.hxx"

/**
 * A buffer that grows while being written to.  It is similar to
 * std::vector, but allows bulk writes.
 */
template<class T>
class GrowingBuffer {
	typedef AllocatedArray<T> Array;

public:
	typedef typename Array::size_type size_type;
	typedef typename Array::iterator iterator;
	typedef typename Array::reference_type reference_type;
	typedef typename Array::const_reference_type const_reference_type;
	typedef typename Array::const_iterator const_iterator;

protected:
	Array array;

	size_type the_size = 0;

public:
	constexpr GrowingBuffer() = default;

	explicit GrowingBuffer(size_type _capacity):Array(_capacity) {}

	GrowingBuffer(GrowingBuffer &&other) = default;

	GrowingBuffer &operator=(GrowingBuffer &&other) {
		array = std::move(other.array);
		return *this;
	}

	constexpr bool empty() const {
		return the_size == 0;
	}

	constexpr size_type size() const {
		return the_size;
	}

	constexpr size_type capacity() const {
		return array.size();
	}

	const_reference_type front() const {
		return array.front();
	}

	/**
	 * Returns one element.  No bounds checking.
	 */
	T &operator[](size_type i) {
		assert(i < size());

		return array[i];
	}

	/**
	 * Returns one constant element.  No bounds checking.
	 */
	const T &operator[](size_type i) const {
		assert(i < size());

		return array[i];
	}

	iterator begin() {
		return array.begin();
	}

	constexpr const_iterator begin() const {
		return array.cbegin();
	}

	iterator end() {
		return std::next(begin(), size);
	}

	constexpr const_iterator end() const {
		return std::next(begin(), size);
	}

	constexpr operator ConstBuffer<T>() const {
		return {&front(), size()};
	}

	T *PrepareWrite(size_type n) {
		if (size() + n > capacity())
			array.GrowPreserve(std::max(capacity() * 2,
						    (capacity() + 2 * n)),
					   size());

		return &array[size()];
	}

	void CommitWrite(size_type n) {
		assert(size() + n <= capacity());

		the_size += n;
	}
};

#endif
