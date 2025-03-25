#include "CyclicArray.h"

#include "gtest/gtest.h"

TEST(CyclicArray, BoundedQueueConstructor) {
	BoundedQueue<int, 3> q;
	EXPECT_TRUE(q.empty());
	BoundedQueue<int, 3> q2 = { 1,2,3 };
	EXPECT_FALSE(q2.empty());
	EXPECT_EQ(q2.front(), 1);
	EXPECT_EQ(q2.back(), 3);
	EXPECT_EQ(q2[1], 2);
}

TEST(CyclicArray, BoundedQueueElementInsertion) {
	BoundedQueue<int, 3> q;
	//front and back well defined after first push
	q.push_front(1);
	EXPECT_EQ(q.front(), 1);
	EXPECT_EQ(q.back(), 1);
	//front and back point to correct items at 2 elem
	q.push_back(2);
	EXPECT_EQ(q.front(), 1);
	EXPECT_EQ(q.back(), 2);
	q.push_front(3);
	EXPECT_EQ(q.front(), 3);
	EXPECT_EQ(q.back(), 2);
	//front overwrites old back
	q.push_front(4);
	EXPECT_EQ(q.front(), 4);
	EXPECT_EQ(q.back(), 1);
	//back overwrites old front
	q.push_back(5);
	EXPECT_EQ(q.front(), 3);
	EXPECT_EQ(q.back(), 5);
}

TEST(CyclicArray, BoundedQueueHeapConstructor) {
	BoundedQueueHeap<int> q(3);
	EXPECT_TRUE(q.empty());
	BoundedQueueHeap<int> q2({ 1,2,3 }, 3);
	EXPECT_FALSE(q2.empty());
	EXPECT_EQ(q2.front(), 1);
	EXPECT_EQ(q2.back(), 3);
	EXPECT_EQ(q2[1], 2);
}

TEST(CyclicArray, BoundedQueueHeapElementInsertion) {
	BoundedQueueHeap<int> q(3);
	//front and back well defined after first push
	q.push_front(1);
	EXPECT_EQ(q.front(), 1);
	EXPECT_EQ(q.back(), 1);
	//front and back point to correct items at 2 elem
	q.push_back(2);
	EXPECT_EQ(q.front(), 1);
	EXPECT_EQ(q.back(), 2);
	q.push_front(3);
	EXPECT_EQ(q.front(), 3);
	EXPECT_EQ(q.back(), 2);
	//front overwrites old back
	q.push_front(4);
	EXPECT_EQ(q.front(), 4);
	EXPECT_EQ(q.back(), 1);
	//back overwrites old front
	q.push_back(5);
	EXPECT_EQ(q.front(), 3);
	EXPECT_EQ(q.back(), 5);
}