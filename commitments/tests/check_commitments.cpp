// Checks of set-off and novation (Proposition 5): the example of the Commitments paper worked exactly, and 500 random graphs of
// obligations, every net obligation kept, the residual acyclic, its volume at least N, and novation leaving exactly N.

#include <thesmos/commitments/commitments.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <cmath>
#include <cstdio>
#include <string>
#include <vector>

using namespace thesmos::commitments;

namespace {
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
}  // namespace

int main() {
  calx::verification::Suite s;
  const Obligations paper{{{"A", "B"}, 6}, {{"B", "C"}, 5}, {{"C", "A"}, 4}};
  const SetOff r = set_off(paper);
  s.require("the common reduction is 4 and leaves A owing B 2 and B owing C 1 (Commitments paper, Equations 7 and 8)",
            r.residual == Obligations{{{"A", "B"}, 2}, {{"B", "C"}, 1}} && r.cycles.size() == 1 && r.cycles[0].second == 4);
  s.require("gross obligations fall from 15 to 3, net obligations stay 2, -1, -1",
            volume(paper) == 15 && volume(r.residual) == 3 && net(r.residual) == net(paper) && net(paper) == std::map<std::string, long long>{{"A", 2}, {"B", -1}, {"C", -1}});
  Obligations later = r.residual;
  later[{"C", "A"}] = 3;
  s.require("a later obligation of 3 from C to A clears to A owing B 1 and C owing A 2", set_off(later).residual == Obligations{{{"A", "B"}, 1}, {{"C", "A"}, 2}});
  const Novation n = novate(r.residual);
  s.require("novation leaves A owing 1 to B and 1 to C, in one step", n.residual == Obligations{{{"A", "B"}, 1}, {{"A", "C"}, 1}} && n.steps == 1);

  calx::random::Stream rng(3, 4, 0, 0);
  int bad = 0;
  double left_setoff = 0, left_novated = 0;
  int counted = 0;
  for (int trial = 0; trial < 500; trial++) {
    std::vector<std::string> names;
    const long long size = between(rng, 3, 9);
    for (long long i = 0; i < size; i++) names.push_back("p" + std::to_string(i));
    Obligations o;
    const long long edges = between(rng, 3, 24);
    for (long long e = 0; e < edges; e++) {
      const long long i = between(rng, 0, size - 1);
      long long j = between(rng, 0, size - 2);
      if (j >= i) j++;
      o[{names[i], names[j]}] += between(rng, 1, 9);
    }
    const long long floor = floor_volume(o);
    const SetOff so = set_off(o);
    bad += net_nonzero(so.residual) != net_nonzero(o);
    bad += !acyclic(so.residual);
    bad += so.cleared + volume(so.residual) != volume(o);
    bad += volume(so.residual) < floor;
    const Novation nv = novate(so.residual);
    std::set<std::string> debtors, creditors;
    for (const auto& [e, a] : nv.residual) debtors.insert(e.first), creditors.insert(e.second);
    for (const auto& d : debtors) bad += creditors.count(d);
    bad += volume(nv.residual) != floor;
    bad += net_nonzero(nv.residual) != net_nonzero(o);
    if (volume(o)) {
      left_setoff += double(volume(so.residual)) / double(volume(o));
      left_novated += double(volume(nv.residual)) / double(volume(o));
      counted++;
    }
  }
  char numbers[160];
  std::snprintf(numbers, sizeof numbers, "set-off leaves on average %.3f of the volume, novation %.3f", left_setoff / counted, left_novated / counted);
  s.require(std::string("on 500 random graphs every net obligation is kept, the residual is acyclic, cleared and left add up, the residual is at least N, "
                        "and after novation nobody both owes and is owed and the volume is exactly N; ") + numbers,
            bad == 0);
  s.require("the shares left agree with the sketch's (0.62 and 0.44, a different random stream) within 0.04",
            std::abs(left_setoff / counted - 0.62) < 0.04 && std::abs(left_novated / counted - 0.44) < 0.04, numbers);
  return s.done();
}
