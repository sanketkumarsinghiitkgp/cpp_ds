#include "gtest/gtest.h"
#include "../src/unordered_set.h"
#include <vector>
#include <algorithm>
TEST(UnorderedSetTest, InitializerList) {
	unordered_set<int> uset({1,2,5,3,2,2});
	std::vector<int> content;
	for(auto &x: uset) {
		content.push_back(x);
	}
	std::sort(content.begin(),content.end());
	EXPECT_EQ(content,std::vector<int> ({1,2,3,5}));
}

TEST(UnorderedSetTest, CopyConstructor) {
	unordered_set<int> uset1({1,2,5,3,2,2});
	std::vector<int> content1;
	for(auto &x: uset1) {
		content1.push_back(x);
	}
	unordered_set<int> uset2(uset1);
	std::vector<int> content2;
	for(auto &x: uset2) {
		content2.push_back(x);
	}
	std::sort(content2.begin(),content2.end());
	std::sort(content1.begin(),content1.end());
	EXPECT_EQ(content1,content2);
}

TEST(UnorderedSetTest, Insert) {
	unordered_set<int> uset;
	std::vector<int> insertionValues({1,2,3,1,1,6,4,});
	for(auto &x: insertionValues) {
		uset.insert(x);
	}
	std::vector<int> content;
	for(auto &x:uset) {
		content.push_back(x);
	}
	std::sort(content.begin(),content.end());
	EXPECT_EQ(content,std::vector<int>({1,2,3,4,6}));
}

TEST(UnorderedSetTest, Erase) {
	unordered_set<int> uset({1,2,5,3,2,2});
	uset.erase(4);
	uset.erase(2);
	std::vector<int> content;
	for(auto &x: uset) {
		content.push_back(x);
	}
	std::sort(content.begin(),content.end());
	EXPECT_EQ(content, std::vector<int>({1,3,5}));
}

TEST(UnorderedSetTest, Find) {
	unordered_set<int> uset({1,2,5,3,2,2});
	std::vector<bool> expectedResults({true,true,true,false,true,false});
	for(int i=0;i<expectedResults.size();i++) {
		EXPECT_EQ(uset.find(i+1)!=uset.end(),expectedResults[i]);
	}
}


