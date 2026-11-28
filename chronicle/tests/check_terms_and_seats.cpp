// Checks of orders, terms and seats (decision 0014): a community's order and its external data (the registries it is listed in, its envoys
// written as objects) read, shown in the profile beside its name and, for the external data, only where the part is shown, and inert in
// admission and the fold on 100 random chronicles (Proposition 39); a position's term read with its faults, ending a position by law and
// listed among the rules by law, and equal act by act to the trigger rule it is shorthand for on 200 random chronicles (Proposition 40); a
// body's seats read with their faults, refusing an admission, a join and a founding that would give it more members than its seats, and
// declining an application decided by rule; amendments of seats and terms, one that would leave a body beyond its seats refused; on 200
// random chronicles no body with seats ever holds more members than its seats, every join refused exactly when an independent count says
// so (Proposition 41); and seats and terms never reached change no act's verdict (Proposition 42).

#include <thesmos/chronicle/briefing.hpp>
#include <thesmos/chronicle/profile.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <fstream>
#include <set>
#include <sstream>
#include <string>
#include <vector>

using namespace thesmos;
using acts::Act;
using chronicle::State;
using kernel::Kind;
using kernel::Parties;
namespace ch = thesmos::chronicle;
using Json = symploke::jsonld::Json;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields);
  return a;
}
bool has(const std::string& text, const std::string& part) { return text.find(part) != std::string::npos; }
Json workshop() { return symploke::jsonld::parse(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json")); }
// The workshop with changes: (part, key, JSON value) set, a key of "" setting the part itself.
constitutions::Constitution with(const std::vector<std::tuple<std::string, std::string, std::string>>& changes) {
  Json doc = workshop();
  for (const auto& [part, key, value] : changes) {
    if (key.empty()) doc.set(part, symploke::jsonld::parse(value));
    else doc.get(part)->set(key, symploke::jsonld::parse(value));
  }
  return constitutions::read(symploke::jsonld::dump(doc));
}
const std::vector<std::string> kPeople = {"cy", "dee", "eve", "ana", "fay", "gus"};
Parties parties() {
  Parties ps;
  for (const char* p : {"sam", "rae", "cy", "dee", "eve", "ana", "fay", "gus"}) ps.add({p, Kind::person});
  return ps;
}
State founded(const constitutions::Constitution& c) {
  State s(parties(), c);
  Act f = act("sam", "found", 0);
  f.holders = {{"sam", "steward"}, {"rae", "reviewer"}};
  ch::step(s, f);
  return s;
}
bool any_fault(const constitutions::Constitution& c, int condition, const std::string& part) {
  for (const auto& f : constitutions::admission(c))
    if (f.condition == condition && has(f.reason, part)) return true;
  return false;
}
// A random act over the workshop's positions: joins and leavings of contributors, admissions of members by the steward and leavings,
// applications for the writer's position, and the steward's word, at turns that rise by 0 to 2.
Act random_act(calx::random::Stream& r, long long& turn) {
  turn += between(r, 0, 2);
  const std::string who = kPeople[static_cast<std::size_t>(between(r, 0, static_cast<long long>(kPeople.size()) - 1))];
  switch (between(r, 0, 6)) {
    case 0:
    case 1: return act(who, "join", turn, {{"position", "contributor"}});
    case 2: return act(who, "leave", turn, {{"position", "contributor"}});
    case 3: return act("sam", "admit", turn, {{"position", "member"}, {"whom", who}});
    case 4: return act(who, "leave", turn, {{"position", "member"}});
    case 5: return act(who, "apply", turn, {{"id", "ap-" + std::to_string(turn) + "-" + who + "-" + std::to_string(r.below(1000000))}, {"position", "writer"}});
    default: return act("sam", "say", turn);
  }
}
// The writer's position, entered on application while fewer than four hold it.
const std::tuple<std::string, std::string, std::string> kWriterHeld{"positions", "writer", R"({"held by": ["person"]})"};
const std::tuple<std::string, std::string, std::string> kWriterEntry{"boundary", "writer", R"({"entry": {"on application": {"while": "holders fewer than 4"}}, "exit": "free"})"};
std::set<std::string> members_of(const State& s, const std::vector<std::string>& of) {
  std::set<std::string> m;
  for (const auto& [key, since] : s.positions)
    if (std::find(of.begin(), of.end(), key.second) != of.end()) m.insert(key.first);
  return m;
}
}  // namespace

int main() {
  calx::verification::Suite s;

  // ---- 1. The order and the external data, read and shown ----
  {
    const auto c = with({{"order", "", R"("commons")"},
                         {"external", "", R"({"card": true, "listed in": ["a league/registry", "a commons of listings/registry"], "compacts": [],
                                              "envoys": ["steward", {"to": "a league", "holder": "sam", "position": "steward"}]})"}});
    State st = founded(c);
    const auto pub = ch::profile(st);
    const auto steward = ch::brief(st, "sam");
    s.require("a community's order and external data are read: its order a label, the registries it says it is listed in, an envoy written as a position as older "
              "records write it and one written as an object naming where it is sent, who holds the mission and its position",
              c.order == "commons" && c.listed_in == std::vector<std::string>{"a league/registry", "a commons of listings/registry"} && c.envoys.size() == 2 &&
                  c.envoys[0].position == "steward" && c.envoys[0].to.empty() && c.envoys[1].to == "a league" && c.envoys[1].holder == "sam" && c.envoys[1].position == "steward");
    const auto hidden = with({{"order", "", R"("commons")"}, {"external", "", R"({"listed in": ["a league/registry"], "envoys": [{"to": "a league", "holder": "sam"}]})"},
                              {"records", "", R"({"captured": ["acts of force"], "visibility": {"external": {"office": ["steward"]}}})"}});
    State hs = founded(hidden);
    const auto outsider = ch::profile(hs), insider = ch::profile(hs, "sam");
    s.require("the public profile shows the order beside the name, and the registries and envoys where the part external is public; kept to the steward's office, "
              "the outsider sees the order and neither, the steward both; the briefing gives the order",
              pub.order == "commons" && pub.listed_in.size() == 2 && pub.envoys.size() == 2 && outsider.order == "commons" && outsider.listed_in.empty() &&
                  outsider.envoys.empty() && insider.listed_in == std::vector<std::string>{"a league/registry"} && insider.envoys.size() == 1 && steward.order == "commons");
    const auto numbered = with({{"order", "", "7"}});
    s.require("an order written as no text is read as absent, and admission is as it was", numbered.order.empty() && constitutions::admission(numbered).empty() && constitutions::admission(c).empty());
  }

  // ---- 2. Proposition 39: the order and the external data are inert ----
  {
    const auto plain = with({kWriterHeld, kWriterEntry});
    const auto labelled = with({kWriterHeld, kWriterEntry, {"order", "", R"("treaty organisation")"},
                                {"external", "", R"({"listed in": ["r1", "r2"], "envoys": [{"to": "x", "holder": "sam"}, "steward"], "compacts": []})"}});
    calx::random::Stream r(20261127, 40, 0, 0);
    bool same = true;
    std::size_t acts_run = 0, refused = 0;
    for (int k = 0; k < 100 && same; k++) {
      State a = founded(plain), b = founded(labelled);
      long long turn = 0;
      for (int i = 0; i < 30 && same; i++) {
        const Act x = random_act(r, turn);
        const auto oa = ch::step(a, x), ob = ch::step(b, x);
        same = oa.admitted == ob.admitted && oa.reason == ob.reason && ch::relational_digest(a) == ch::relational_digest(b);
        acts_run++, refused += !oa.admitted;
      }
    }
    s.require("on 100 random chronicles (" + std::to_string(acts_run) + " acts, " + std::to_string(refused) + " refused) a constitution with an order, registries and envoys "
              "admits and refuses every act as the same constitution without them, with the same relational digest (Proposition 39)",
              same && refused > 0 && refused < acts_run);
  }

  // ---- 3. A position's term ----
  {
    const auto termed = with({{"boundary", "contributor", R"({"entry": "open", "exit": "free", "term": 3})"}});
    bool faults = true;
    for (const char* bad : {"0", "2.5", R"("three")", "-1"})
      faults = faults && any_fault(with({{"boundary", "contributor", std::string(R"({"entry": "open", "exit": "free", "term": )") + bad + "}"}}), 1, "the term of the position contributor");
    s.require("a position's term is read in turns, and a term of nought, a fraction, a word or a negative number is a fault of closure naming the position",
              termed.boundary.at("contributor").term == 3 && constitutions::admission(termed).empty() && faults);
    State st = founded(termed);
    const auto joined = ch::step(st, act("ana", "join", 1, {{"position", "contributor"}}));
    const auto at4 = ch::step(st, act("sam", "say", 4));
    const bool held4 = st.holds("ana", "contributor");
    const bool view = !ch::settled(st, 5).holds("ana", "contributor") && st.holds("ana", "contributor");
    const auto at5 = ch::step(st, act("sam", "say", 5));
    const bool ended5 = !st.holds("ana", "contributor");
    const auto b = ch::brief(st, "sam");
    const bool listed = std::find(b.by_law.begin(), b.by_law.end(), "the term of the position contributor: end position contributor held for more than 3 turns") != b.by_law.end();
    State again;
    const auto replayed = ch::replay(st.parties, termed, st.acts, st.journal, &again);
    const bool alike = replayed.holds() && ch::relational_digest(again) == ch::relational_digest(st);
    const auto rejoin = ch::step(st, act("ana", "join", 6, {{"position", "contributor"}}));
    s.require("a contributor's term of three turns: joined at turn 1, held at turn 4, ended by law at turn 5 (the settled view shows it before any act), listed among the "
              "rules by law in the briefing; a replay settles alike; the contributor may join again for a new term",
              joined.admitted && at4.admitted && held4 && view && at5.admitted && ended5 && listed && alike && rejoin.admitted && st.positions.at({"ana", "contributor"}) == 6,
              std::string(joined.admitted ? "" : "joined ") + (at4.admitted ? "" : "at4 ") + (held4 ? "" : "held4 ") + (view ? "" : "view ") + (at5.admitted ? "" : "at5 ") +
                  (listed ? "" : "listed ") + (rejoin.admitted ? "" : "rejoin " + rejoin.reason) + (b.by_law.empty() ? "" : b.by_law.front()));
  }

  // ---- 4. Proposition 40: a term is the trigger rule it is shorthand for ----
  {
    const auto termed = with({kWriterHeld, {"boundary", "writer", R"({"entry": {"on application": {"while": "holders fewer than 4"}}, "exit": "free", "term": 2})"},
                              {"boundary", "contributor", R"({"entry": "open", "exit": "free", "term": 3})"},
                              {"boundary", "member", R"({"entry": {"decided by": "admit"}, "exit": "free", "term": 4})"}});
    const auto declared = with({kWriterHeld, kWriterEntry, {"behaviour", "", R"({"format": "noetarium.behaviour/1", "name": "terms", "version": 1, "extends": "base 5",
      "triggers": [{"rule": "the writer's term", "when": {"position": "writer", "held for more than": 2}, "then": "end position"},
                   {"rule": "the contributor's term", "when": {"position": "contributor", "held for more than": 3}, "then": "end position"},
                   {"rule": "the member's term", "when": {"position": "member", "held for more than": 4}, "then": "end position"}]})"}});
    s.require("the constitution with terms and the constitution declaring the three trigger rules are both admitted, the first holding three term rules",
              constitutions::admission(termed).empty() && constitutions::admission(declared).empty() && constitutions::term_triggers(termed).size() == 3 &&
                  constitutions::triggers(termed).size() == constitutions::triggers(declared).size(),
              constitutions::admission(declared).empty() ? "" : constitutions::admission(declared).front().reason);
    calx::random::Stream r(20261127, 41, 0, 0);
    bool same = true;
    std::size_t ended = 0;
    for (int k = 0; k < 200 && same; k++) {
      State a = founded(termed), b = founded(declared);
      long long turn = 0;
      for (int i = 0; i < 30 && same; i++) {
        const Act x = random_act(r, turn);
        const auto before = a.positions.size();
        const auto oa = ch::step(a, x), ob = ch::step(b, x);
        same = oa.admitted == ob.admitted && oa.reason == ob.reason && a.positions == b.positions;
        if (oa.admitted && a.positions.size() < before && x.kind != "leave") ended++;
      }
    }
    s.require("on 200 random chronicles a position's term and the declared trigger rule \"a position held for more than n turns ends\" admit and refuse every act alike "
              "and leave every position alike at every prefix (" + std::to_string(ended) + " positions ended by law; Proposition 40)",
              same && ended > 0);
  }

  // ---- 5. A body's seats ----
  {
    const auto seated = with({{"positions", "councillor", R"({"held by": ["person"]})"},
                              {"boundary", "councillor", R"({"entry": {"decided by": "admit"}, "exit": "free"})"},
                              {"bodies", "council", R"({"of": ["councillor"], "seats": 2})"}});
    bool faults = true;
    for (const char* bad : {"0", "1.5", R"("two")"})
      faults = faults && any_fault(with({{"bodies", "stewards", std::string(R"({"of": ["steward"], "seats": )") + bad + "}"}}), 2, "the seats of the body stewards");
    s.require("a body's seats are read, and seats of nought, a fraction or a word are a fault of condition 2 naming the body",
              seated.bodies.at("council").seats == 2 && constitutions::admission(seated).empty() && faults);
    State st = founded(seated);
    const auto a1 = ch::step(st, act("sam", "admit", 1, {{"position", "councillor"}, {"whom", "ana"}}));
    const auto a2 = ch::step(st, act("sam", "admit", 1, {{"position", "councillor"}, {"whom", "fay"}}));
    const auto a3 = ch::step(st, act("sam", "admit", 1, {{"position", "councillor"}, {"whom", "gus"}}));
    const auto out = ch::step(st, act("ana", "leave", 2, {{"position", "councillor"}}));
    const auto a4 = ch::step(st, act("sam", "admit", 2, {{"position", "councillor"}, {"whom", "gus"}}));
    s.require("a council of two seats: two councillors admitted, a third refused with the body and its seats named; once one leaves, the third admitted",
              a1.admitted && a2.admitted && !a3.admitted && has(a3.reason, "the body council has 2 seats") && has(a3.reason, "3 members") && out.admitted && a4.admitted,
              a3.reason);

    const auto floor = with({{"bodies", "floor", R"({"of": ["contributor"], "seats": 2})"}});
    State fl = founded(floor);
    const auto j1 = ch::step(fl, act("cy", "join", 1, {{"position", "contributor"}}));
    const auto j2 = ch::step(fl, act("dee", "join", 1, {{"position", "contributor"}}));
    const auto j3 = ch::step(fl, act("eve", "join", 1, {{"position", "contributor"}}));
    const auto again = ch::step(fl, act("cy", "join", 2, {{"position", "contributor"}}));
    State crowd(parties(), floor);
    Act f = act("sam", "found", 0);
    f.holders = {{"sam", "steward"}, {"cy", "contributor"}, {"dee", "contributor"}, {"eve", "contributor"}};
    const auto founding = ch::step(crowd, f);
    s.require("a body of two seats over an open position: two contributors join, a third is refused, one who sits already joins again; a founding that names three "
              "contributors is refused",
              j1.admitted && j2.admitted && !j3.admitted && has(j3.reason, "the body floor has 2 seats") && again.admitted && !founding.admitted && has(founding.reason, "floor"),
              j3.reason + " / " + founding.reason);

    const auto writers = with({kWriterHeld, kWriterEntry, {"bodies", "writers", R"({"of": ["writer"], "seats": 1})"}});
    State ws = founded(writers);
    const auto w1 = ch::step(ws, act("ana", "apply", 1, {{"id", "ap1"}, {"position", "writer"}}));
    const auto w2 = ch::step(ws, act("fay", "apply", 1, {{"id", "ap2"}, {"position", "writer"}}));
    s.require("an application decided by rule while the writers' one seat is filled is admitted and declined, its finding naming the seats; the first is admitted",
              w1.admitted && w2.admitted && ws.items.at("ap1").status == "admitted" && ws.items.at("ap2").status == "declined" &&
                  has(ws.items.at("ap2").finding, "the body writers has 1 seat") && ws.holds("ana", "writer") && !ws.holds("fay", "writer"),
              ws.items.count("ap2") ? ws.items.at("ap2").finding : w2.reason);
  }

  // ---- 6. Amendments of seats and terms ----
  {
    const auto seated = with({{"positions", "councillor", R"({"held by": ["person"]})"},
                              {"boundary", "councillor", R"({"entry": {"decided by": "admit"}, "exit": "free"})"},
                              {"bodies", "council", R"({"of": ["councillor"], "seats": 2})"}});
    State st = founded(seated);
    ch::step(st, act("sam", "admit", 1, {{"position", "councillor"}, {"whom", "ana"}}));
    ch::step(st, act("sam", "admit", 1, {{"position", "councillor"}, {"whom", "fay"}}));
    auto amend = [&](const std::string& id, const std::string& key, const std::string& value, long long turn) {
      ch::step(st, act("sam", "propose", turn, {{"id", id}, {"decision", "amend operational"}, {"change." + key, value}}));
      ch::step(st, act("sam", "vote", turn, {{"id", id}, {"value", "yes"}}));
      ch::step(st, act("rae", "vote", turn, {{"id", id}, {"value", "yes"}}));
      return ch::step(st, act("sam", "amend", turn + 1, {{"id", id}}));
    };
    const auto fewer = amend("s1", "seats", R"({"council": 1})", 2);
    const auto more = amend("s2", "seats", R"({"council": 3})", 4);
    const auto third = ch::step(st, act("sam", "admit", 6, {{"position", "councillor"}, {"whom", "gus"}}));
    const auto termed = amend("t1", "term", R"({"councillor": 5})", 7);
    const auto unknown = amend("s3", "seats", R"({"senate": 2})", 9);
    const auto at13 = ch::step(st, act("sam", "say", 13));
    s.require("an amendment that would leave the council of two members with one seat is refused; one raising its seats to three is adopted and a third admitted; a "
              "term of five turns given to councillors by amendment ends their positions by law; seats for a body the constitution lacks are refused",
              !fewer.admitted && has(fewer.reason, "would leave it with 2 members") && more.admitted && third.admitted && termed.admitted &&
                  st.rules.boundary.at("councillor").term == 5 && !unknown.admitted && has(unknown.reason, "senate") && at13.admitted && !st.holds("ana", "councillor") &&
                  !st.holds("gus", "councillor"),
              fewer.reason + " / " + unknown.reason);
  }

  // ---- 7. Proposition 41: a body never holds more members than its seats ----
  {
    const auto seated = with({kWriterHeld, kWriterEntry, {"bodies", "floor", R"({"of": ["contributor"], "seats": 3})"},
                              {"bodies", "council", R"({"of": ["member", "writer"], "seats": 2})"}});
    calx::random::Stream r(20261127, 42, 0, 0);
    bool within = true, predicted = true;
    std::size_t joins = 0, full = 0, amended = 0;
    for (int k = 0; k < 200 && within && predicted; k++) {
      State st = founded(seated);
      long long turn = 0;
      for (int i = 0; i < 40 && within && predicted; i++) {
        Act x = random_act(r, turn);
        if (between(r, 0, 9) == 0) {  // an amendment of the floor's seats, proposed, voted and amended at the next turn
          const std::string id = "seats-" + std::to_string(k) + "-" + std::to_string(i);
          ch::step(st, act("sam", "propose", turn, {{"id", id}, {"decision", "amend operational"}, {"change.seats", R"({"floor": )" + std::to_string(between(r, 1, 4)) + "}"}}));
          ch::step(st, act("sam", "vote", turn, {{"id", id}, {"value", "yes"}}));
          ch::step(st, act("rae", "vote", turn, {{"id", id}, {"value", "yes"}}));
          x = act("sam", "amend", ++turn, {{"id", id}});
        }
        // An independent reading of a join: refused exactly when the floor would then hold more members than its seats.
        bool expect = true;
        if (x.kind == "join") {
          std::set<std::string> m = members_of(st, st.rules.bodies.at("floor").of);
          const std::size_t before = m.size();
          m.insert(x.author);
          expect = !(m.size() > static_cast<std::size_t>(st.rules.bodies.at("floor").seats) && m.size() > before);
        }
        const auto o = ch::step(st, x);
        if (x.kind == "join") joins++, full += !o.admitted, predicted = o.admitted == expect;
        if (x.kind == "amend" && o.admitted) amended++;
        for (const auto& [name, body] : st.rules.bodies)
          if (body.seats > 0) within = within && members_of(st, body.of).size() <= static_cast<std::size_t>(body.seats);
      }
    }
    s.require("on 200 random chronicles of joins, admissions, applications, leavings and amendments of seats (" + std::to_string(joins) + " joins, " + std::to_string(full) +
                  " refused for want of a seat, " + std::to_string(amended) + " amendments adopted) no body ever holds more members than its seats, and every join is "
                  "refused exactly when an independent count says the floor would pass its seats (Proposition 41)",
              within && predicted && full > 0 && amended > 0);
  }

  // ---- 8. Proposition 42: seats and terms never reached change nothing ----
  {
    const auto plain = with({kWriterHeld, kWriterEntry});
    const auto roomy = with({kWriterHeld, {"boundary", "writer", R"({"entry": {"on application": {"while": "holders fewer than 4"}}, "exit": "free", "term": 1000})"},
                             {"bodies", "floor", R"({"of": ["contributor"], "seats": 1000})"}, {"bodies", "stewards", R"({"of": ["steward"], "seats": 1000})"}});
    calx::random::Stream r(20261127, 43, 0, 0);
    bool same = true;
    for (int k = 0; k < 100 && same; k++) {
      State a = founded(plain), b = founded(roomy);
      long long turn = 0;
      for (int i = 0; i < 30 && same; i++) {
        const Act x = random_act(r, turn);
        const auto oa = ch::step(a, x), ob = ch::step(b, x);
        same = oa.admitted == ob.admitted && oa.reason == ob.reason && a.positions == b.positions;
      }
    }
    s.require("on 100 random chronicles seats and a term never reached admit and refuse every act as the constitution without them, and a constitution that declares "
              "neither holds no trigger rule beyond its taxonomy's (Proposition 42)",
              same && constitutions::term_triggers(plain).empty() && constitutions::triggers(plain).size() == plain.behaviour.triggers.size());
  }
  return s.done();
}
