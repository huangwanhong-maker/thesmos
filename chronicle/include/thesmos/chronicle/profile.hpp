// The profile of a community: what a reader may know of it from its record without joining.
//
// A community publishes what an outsider needs to find it and judge whether to join: its mission, the parts of its constitution its
// policy of records makes public, its offices and their holders, its posted work, its admission policy, the holdings it makes public
// and the norms of access to them, its compacts, and its route for claims (decision 0006; the design record of 2026-11-27,
// "Communities That Agents Find, Join and Leave"). The public profile, read with no reader, is what a directory lists; read for a
// party, the profile shows also what that party's positions let it see. The purpose, the boundary rules and the route for claims
// are always shown (G8). The ground fixes no policy of what is public: each community declares its own.

#pragma once

#include <thesmos/chronicle/chronicle.hpp>

#include <map>
#include <set>
#include <string>
#include <vector>

namespace thesmos::chronicle {

struct Office {
  std::string position;
  std::vector<std::string> holders;
};

struct Posted {
  std::string kind;  // request (work asked for), offer (work offered, awaiting taking)
  std::string id, matter, by;
  long long turn = 0;
};

struct Profile {
  std::string name, purpose;
  // Decision 0014: the community's order, a label always shown beside its name, which registries read for search; and, when the part
  // "external" is shown, the registries it says it is listed in and its envoys as it writes them. None of it is read by the fold.
  std::string order;
  std::vector<std::string> listed_in;
  std::vector<constitutions::Envoy> envoys;
  std::set<std::string> reader_positions;           // the positions the reader holds (none for the public)
  std::set<std::string> shown, withheld;            // the parts of the constitution and of the state shown to the reader, and kept from it
  Constitution rules;                               // the constitution in force, of which the shown parts are read
  std::vector<Office> offices;                      // when "holders" is shown
  std::vector<Posted> work;                         // when "work" is shown: the requests in the record and the offers open
  std::vector<thesmos::holdings::Holding> holdings;  // when "holdings" is shown: those whose visibility the reader meets
  // Items, routes and rulings (decision 0011): the routes (always shown), the register of rulings when shown, and for each type of item
  // how many are open and how many of them are overdue, a mark every reader sees.
  std::vector<Ruling> rulings;
  std::map<std::string, std::pair<long long, long long>> items;  // type -> (open, overdue)
  // Posted work (decision 0012), when "work" is shown: each item with its kind, state, poster, due turn and overdue mark.
  struct PostedWork {
    std::string id, kind, state, poster, matter;
    long long posted = 0, due = -1;
    bool overdue = false;
  };
  std::vector<PostedWork> posted_work;
};

// The profile of a community for a reader (a party of the community or none, for the public).
inline Profile profile(const State& s, const std::string& reader = {}) {
  Profile p;
  p.name = s.rules.name, p.purpose = s.rules.purpose, p.rules = s.rules;
  if (!reader.empty())
    for (const auto& [key, since] : s.positions)
      if (key.first == reader) p.reader_positions.insert(key.second);
  for (const auto& part : constitutions::parts()) (thesmos::holdings::sees(constitutions::shown(s.rules, part), p.reader_positions) ? p.shown : p.withheld).insert(part);
  p.order = s.rules.order;
  if (p.shown.count("external")) p.listed_in = s.rules.listed_in, p.envoys = s.rules.envoys;
  if (p.shown.count("holders"))
    for (const auto& [position, kinds] : s.rules.positions) {
      Office o{position, {}};
      for (const auto& [key, since] : s.positions)
        if (key.second == position) o.holders.push_back(key.first);
      p.offices.push_back(o);
    }
  if (p.shown.count("work")) {
    for (const Act& a : s.acts)
      if (a.kind == "request" && !a.has("type") && !a.has("work")) p.work.push_back({"request", a.field("id"), a.field("matter"), a.author, a.turn});
    for (const auto& [id, c] : s.commitments)
      if (c.status == Status::conditional) p.work.push_back({"offer", id, c.matter, c.debtor, 0});
  }
  if (p.shown.count("holdings"))
    for (const auto& [id, h] : s.holdings)
      if (!h.withdrawn && thesmos::holdings::sees(h.visibility, p.reader_positions)) p.holdings.push_back(h);
  if (p.shown.count("rulings"))
    for (const auto& [id, r] : s.rulings) p.rulings.push_back(r);
  const long long now = s.acts.empty() ? 0 : s.acts.back().turn;
  if (p.shown.count("work"))
    for (const auto& [id, i] : s.work_items) {
      const std::string st = work_state(s, i, now);
      p.posted_work.push_back({id, i.kind, st, i.poster, i.matter, i.posted, i.due, i.due >= 0 && now > i.due && st != "accepted" && st != "withdrawn"});
    }
  for (const auto& [id, i] : s.items)
    if (i.status == "open") {
      auto& n = p.items[i.type];
      n.first++, n.second += overdue(i, now);
    }
  return p;
}

}  // namespace thesmos::chronicle
