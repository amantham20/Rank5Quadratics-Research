

#include <omp.h>
#include <iostream>
#include <queue>

int main() {

  using Bucket = std::pair<size_t, size_t>;

  auto cmp = [](const Bucket& a, const Bucket& b) { return a.first > b.first; };
  std::priority_queue<Bucket, std::vector<Bucket>, decltype(cmp)> top_buckets(cmp);

  for (int i = 0; i < 1000; ++i) {
    if (top_buckets.size() < 10) {
      top_buckets.push(std::make_pair(i, i));
    } else if (top_buckets.top().first < i) {
      top_buckets.pop();
      top_buckets.push(std::make_pair(i, i));
    }
  }

  std::vector<Bucket> top_10;
  while (!top_buckets.empty()) {
    top_10.push_back(top_buckets.top());
    top_buckets.pop();
  }

  std::cout << "Top 10 buckets: " << std::endl;
  for (int i = top_10.size() - 1; i >= 0; --i) {
    std::cout << "Bucket: " << top_10[i].second << " Count: " << top_10[i].first << std::endl;
  }
}