// Checks of disclosure agreements and epistemic behaviour recorded by agreement (decision 0010): an agreement ratified by each side under
// its own rule (the principal by an act it answers for, the community by an office its choice rules empower to agree), in force once
// both ratified the same terms, its record held as a holding of kind "epistemic record" with the norms of access its terms declare;
// refusals of terms that do not stand or do not fit the community, of a side ratifying twice or on other terms, of the community's side
// by one without the office; epistemic episodes disclosed under an agreement that covers them, as their digest and size, and outside
// any agreement only as the fact that reasoning took place; consults of the record for the purposes admitted, refused for those never
// admitted, for no purpose, by one the terms do not let consult; an act citing the record only after a consult for its purpose; a
// denunciation effective after its notice, what was recorded then kept, returned or erased; on 100 random runs the journal keeps
// epistemic content only under agreements that cover it, as an independent reckoning says, and never the content itself (Proposition
// 24); on 300 random consults the fold decides as an independent reading of the terms does (Proposition 25); and a record with
// agreements replays, and two journals that differ only in their epistemic entries leave one state.

#include <thesmos/chronicle/conduct.hpp>

#include <calx/random/stream.hpp>
#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using namespace thesmos;
using acts::Act;
using chronicle::State;
using kernel::Kind;
using kernel::Parties;
namespace ch = thesmos::chronicle;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
long long between(calx::random::Stream& r, long long lo, long long hi) { return lo + static_cast<long long>(r.below(static_cast<std::uint64_t>(hi - lo + 1))); }
Act act(std::string author, std::string kind, long long turn, std::map<std::string, std::string> fields = {}, std::string mandate = {}) {
  Act a;
  a.author = std::move(author), a.kind = std::move(kind), a.turn = turn, a.fields = std::move(fields), a.mandate = std::move(mandate);
  return a;
}
// The illustrative workshop, its steward empowered to agree.
constitutions::Constitution host() {
  auto doc = symploke::jsonld::parse(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
  doc.get("choice")->items.push_back(symploke::jsonld::parse(R"({"who": "steward", "may": ["agree"]})"));
  return constitutions::read(symploke::jsonld::dump(doc));
}
Parties parties() {
  Parties ps;
  for (const char* p : {"sam", "rae", "cy", "ana", "zed"}) ps.add({p, Kind::person});
  ps.add({"ana/scribe", Kind::operative, "ana", "provider-a"});
  ps.add({"ana/clerk", Kind::operative, "ana", "provider-b"});
  ps.add({"zed/agent", Kind::operative, "zed", "provider-a"});
  return ps;
}
State founded() {
  State s(parties(), host());
  Act f = act("sam", "found", 0);
  f.holders = {{"sam", "steward"}, {"rae", "reviewer"}, {"cy", "contributor"}};
  ch::step(s, f);
  return s;
}
struct Spec {
  std::string id = "scribe reasoning", principal = "ana", agents = R"(["ana/scribe"])", kinds = R"(["reasoning", "model call"])", matters = "[]";
  long long from = 0, to = -1, retention = 4, notice = 2, ceiling = -1;
  std::string admits = R"(["review", "good faith"])", never = R"(["intent"])", holding = "scribe's reasoning", custodian = "steward", body = "stewards", then = "erased",
              by = "either";
  bool principal_reads = true;
};
std::string terms(const Spec& x) {
  return std::string(R"({"format": "noetarium.disclosure/1", "id": ")") + x.id + R"(", "principal": ")" + x.principal + R"(", "agents": )" + x.agents +
         R"(, "scope": {"kinds": )" + x.kinds + R"(, "matters": )" + x.matters + R"(, "from": )" + std::to_string(x.from) +
         (x.to >= 0 ? R"(, "to": )" + std::to_string(x.to) : std::string()) + R"(}, "purpose": {"admits": )" + x.admits + R"(, "never": )" + x.never +
         R"(}, "custody": {"holding": ")" + x.holding + R"(", "custodian": ")" + x.custodian + R"(", "retention": )" + std::to_string(x.retention) +
         R"(}, "access": {"principal": )" + (x.principal_reads ? "true" : "false") + (x.body.empty() ? std::string() : R"(, "body": ")" + x.body + "\"") +
         R"(}, "cost": {"borne by": "principal", "unit": "bytes")" + (x.ceiling >= 0 ? R"(, "ceiling": )" + std::to_string(x.ceiling) : std::string()) +
         R"(}, "withdrawal": {"by": ")" + x.by + R"(", "notice": )" + std::to_string(x.notice) + R"(, "then": ")" + x.then + R"("}})";
}
Act agreeing(const std::string& author, const std::string& side, const Spec& x, long long turn) {
  return act(author, "agree", turn, {{"agreement", x.id}, {"for", side}, {"terms", terms(x)}});
}
Act consulting(const std::string& who, const std::string& holding, const std::string& purpose, long long turn) {
  Act a = act(who, "consult", turn, {{"holding", holding}});
  if (!purpose.empty()) a.fields["purpose"] = purpose;
  return a;
}
ch::Epistemic thought(const std::string& party, const std::string& kind, long long turn, const std::string& content, long long size = 10, const std::string& matter = {}) {
  ch::Epistemic e;
  e.party = party, e.kind = kind, e.turn = turn, e.content = content, e.size = size, e.matter = matter;
  return e;
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const Spec plain;

  // ---- 1. An agreement ratified by each side under its own rule ------------------------------------------------------------------------
  {
    State st = founded();
    const auto p = ch::step(st, agreeing("ana", "principal", plain, 1));
    const bool waiting = !st.holdings.count(plain.holding) && !ch::in_force(st.agreements.at(plain.id), 1);
    const auto c = ch::step(st, agreeing("sam", "community", plain, 2));
    const auto& h = st.holdings.at(plain.holding);
    s.require("the principal ratifies by its own act, though no position of the community empowers it, and the steward for the community; the agreement stands in force "
              "from the second ratification, its record held as an epistemic record kept by the stewards, with a permission and a prohibition for each who may consult it",
              p.admitted && waiting && c.admitted && ch::in_force(st.agreements.at(plain.id), 2) && h.kind == "epistemic record" && h.custodian == "steward" && h.office &&
                  st.rules.holdings.norms.size() >= 4 && st.records.at(plain.holding) == plain.id,
              p.reason + " / " + c.reason);
    State a = founded(), b = founded(), d = founded(), e = founded();
    ch::step(a, agreeing("ana", "principal", plain, 1));
    const auto by_member = ch::step(a, agreeing("cy", "community", plain, 2));
    Spec other = plain;
    other.retention = 9;
    const auto other_terms = ch::step(a, agreeing("sam", "community", other, 2));
    const auto twice = ch::step(a, agreeing("ana", "principal", plain, 2));
    const auto for_another = ch::step(b, agreeing("zed", "principal", plain, 1));
    const auto no_side = ch::step(b, act("sam", "agree", 1, {{"agreement", plain.id}, {"terms", terms(plain)}}));
    s.require("refused: the community's side by one without the office, a second ratification on other terms, a side ratifying twice, the principal's side by another, an act "
              "naming no side",
              !by_member.admitted && !other_terms.admitted && other_terms.reason.find("other terms") != std::string::npos && !twice.admitted && !for_another.admitted &&
                  !no_side.admitted,
              by_member.reason + " / " + other_terms.reason + " / " + twice.reason + " / " + for_another.reason);
    Spec both = plain, kind = plain, agent = plain, body = plain, held = plain;
    both.never = R"(["review"])";
    kind.kinds = R"(["dreaming"])";
    agent.agents = R"(["zed/agent"])";
    body.body = "council";
    held.holding = "sources";
    const auto r1 = ch::step(d, agreeing("ana", "principal", both, 1)), r2 = ch::step(d, agreeing("ana", "principal", kind, 1)), r3 = ch::step(d, agreeing("ana", "principal", agent, 1)),
               r4 = ch::step(d, agreeing("ana", "principal", body, 1)), r5 = ch::step(e, agreeing("ana", "principal", held, 1));
    s.require("refused: terms that admit and never admit one use, a kind that is no epistemic behaviour, an agent the principal does not control, a body the community lacks, "
              "a record kept as a holding the community holds already",
              !r1.admitted && r1.reason.find("both admits") != std::string::npos && !r2.admitted && !r3.admitted && r3.reason.find("zed/agent") != std::string::npos &&
                  !r4.admitted && !r5.admitted,
              r1.reason + " / " + r2.reason + " / " + r3.reason + " / " + r4.reason + " / " + r5.reason);
    // A community whose choice rules name no one to agree cannot ratify.
    auto doc = symploke::jsonld::parse(slurp(std::string(THESMOS_DATA) + "/examples/workshop.json"));
    State plain_host(parties(), constitutions::read(symploke::jsonld::dump(doc)));
    Act f = act("sam", "found", 0);
    f.holders = {{"sam", "steward"}};
    ch::step(plain_host, f);
    ch::step(plain_host, agreeing("ana", "principal", plain, 1));
    const auto none = ch::step(plain_host, agreeing("sam", "community", plain, 2));
    s.require("a community whose choice rules empower no one to agree ratifies no agreement", !none.admitted && none.reason.find("empower to agree") != std::string::npos, none.reason);
  }

  // ---- 2. Epistemic behaviour recorded by agreement -------------------------------------------------------------------------------------
  State st = founded();
  ch::Conduct journal;
  Spec capped = plain;
  capped.to = 20, capped.ceiling = 50, capped.matters = R"(["the catalogue"])";
  {
    const auto before = ch::disclose(st, journal, thought("ana/scribe", "reasoning", 0, "a first thought", 10, "the catalogue"), "000000000000");
    ch::step(st, agreeing("ana", "principal", capped, 1));
    ch::step(st, agreeing("sam", "community", capped, 2));
    const auto in = ch::disclose(st, journal, thought("ana/scribe", "reasoning", 3, "read the catalogue first", 20, "the catalogue"), "000000000003");
    const auto plan = ch::disclose(st, journal, thought("ana/scribe", "plan", 3, "a plan", 5, "the catalogue"), "000000000003");
    const auto clerk = ch::disclose(st, journal, thought("ana/clerk", "reasoning", 3, "the clerk's", 5, "the catalogue"), "000000000003");
    const auto elsewhere = ch::disclose(st, journal, thought("ana/scribe", "model call", 4, "another matter", 5, "the bench"), "000000000004");
    const auto call = ch::disclose(st, journal, thought("ana/scribe", "model call", 4, "prompt and answer", 25, "the catalogue"), "000000000004");
    const auto over = ch::disclose(st, journal, thought("ana/scribe", "reasoning", 5, "too much", 10, "the catalogue"), "000000000005");
    const auto late = ch::disclose(st, journal, thought("ana/scribe", "reasoning", 21, "after its term", 1, "the catalogue"), "000000000021");
    s.require("under the agreement in force the scribe's reasoning and model calls on the catalogue are recorded as their digest; before it, for a plan, for the clerk, on "
              "another matter, past the ceiling and after its last turn only the fact that reasoning took place",
              !before.recorded && in.recorded && in.agreement == plain.id && in.digest == archeion::fixity::sha256_hex("read the catalogue first") && !plan.recorded &&
                  !clerk.recorded && !elsewhere.recorded && call.recorded && !over.recorded && over.reason.find("ceiling") != std::string::npos && !late.recorded,
              over.reason + " / " + late.reason);
    const auto v = ch::disclosed_by_agreement(st, journal);
    bool content = false;
    for (const auto& e : journal.journal.entries) content = content || e.about.find("catalogue first") != std::string::npos || e.about.find("prompt and answer") != std::string::npos;
    s.require("the journal keeps epistemic behaviour only under the agreement that covers it, and never the content itself", v.holds() && !content, v.reason);
    ch::Conduct forged = journal;
    forged.journal.append({"000000000099", "ana/scribe", "epistemic", std::string("ana/scribe\x1e" "ana\x1ereasoning\x1e\x1ethe catalogue\x1e" "0\x1e") + plain.id + "\x1e" +
                                                                              archeion::fixity::sha256_hex("x") + "\x1e" "1", {}, {}});
    ch::Conduct chatty = journal;
    chatty.journal.append({"000000000099", "ana/scribe", "reasoning", std::string("ana/scribe\x1e" "9\x1eit thought of the catalogue"), {}, {}});
    const auto f1 = ch::disclosed_by_agreement(st, forged), f2 = ch::disclosed_by_agreement(st, chatty);
    s.require("an epistemic entry from a turn when its agreement was not in force, and an entry of reasoning that says more than its party and turn, are each found",
              !f1.holds() && f1.reason.find("not in force") != std::string::npos && !f2.holds(), f1.reason + " / " + f2.reason);
  }

  // ---- 3. Uses of the record ------------------------------------------------------------------------------------------------------------
  {
    const auto review = ch::step(st, consulting("ana", plain.holding, "review", 4));
    const auto intent = ch::step(st, consulting("ana", plain.holding, "intent", 4));
    const auto faith = ch::step(st, consulting("sam", plain.holding, "good faith", 4));
    const auto aimless = ch::step(st, consulting("sam", plain.holding, "", 4));
    const auto stranger = ch::step(st, consulting("cy", plain.holding, "review", 4));
    const auto deposit = ch::step(st, act("sam", "deposit", 4, {{"holding", plain.holding}, {"amount", "1"}}));
    s.require("the principal consults the record for review, the steward for good faith, each consult recorded with its norm; refused: a use for intent, which the "
              "agreement never admits, a consult for no purpose, a consult by one the terms do not let consult, and any act on the record but a consult",
              review.admitted && !intent.admitted && intent.reason.find("forbids") != std::string::npos && faith.admitted && !aimless.admitted && !stranger.admitted &&
                  !deposit.admitted && st.accesses.size() == 2 && st.accesses.back().purpose == "good faith" && st.accesses.back().norm.rfind("agreement ", 0) == 0,
              review.reason + " / " + intent.reason + " / " + aimless.reason + " / " + stranger.reason + " / " + deposit.reason);
    const auto cites = ch::step(st, act("sam", "assess", 5, {{"object", "ana/scribe"}, {"criterion", "good faith"}, {"value", "shown"}, {"epistemic record", plain.holding}, {"purpose", "good faith"}}));
    const auto for_intent = ch::step(st, act("sam", "assess", 5, {{"object", "ana/scribe"}, {"criterion", "intent"}, {"value", "malice"}, {"epistemic record", plain.holding}, {"purpose", "intent"}}));
    const auto unread = ch::step(st, act("rae", "assess", 5, {{"object", "ana/scribe"}, {"criterion", "good faith"}, {"value", "shown"}, {"epistemic record", plain.holding}, {"purpose", "good faith"}}));
    s.require("the steward cites the record as evidence of good faith after consulting it for that use; refused: a citation for intent, and one by a party that never consulted it",
              cites.admitted && !for_intent.admitted && for_intent.reason.find("does not admit") != std::string::npos && !unread.admitted && unread.reason.find("follows a consult") != std::string::npos,
              cites.reason + " / " + for_intent.reason + " / " + unread.reason);
  }

  // ---- 4. Denunciation and the end of an agreement -------------------------------------------------------------------------------------
  {
    const auto by_member = ch::step(st, act("cy", "denounce", 6, {{"agreement", plain.id}, {"for", "community"}}));
    const auto denounced = ch::step(st, act("ana", "denounce", 6, {{"agreement", plain.id}, {"for", "principal"}}));
    const auto during = ch::step(st, consulting("ana", plain.holding, "review", 7));
    const auto still = ch::disclose(st, journal, thought("ana/scribe", "reasoning", 7, "during the notice", 1, "the catalogue"), "000000000007");
    const auto after = ch::step(st, consulting("ana", plain.holding, "review", 8));
    const auto gone = ch::disclose(st, journal, thought("ana/scribe", "reasoning", 8, "after the end", 1, "the catalogue"), "000000000008");
    s.require("refused: a denunciation for the community by one without the office; the principal's denunciation takes effect after two turns of notice, the record consulted and "
              "added to meanwhile, and erased once it has ended, after which only that reasoning took place is recorded",
              !by_member.admitted && denounced.admitted && during.admitted && still.recorded && !after.admitted && after.reason.find("erased") != std::string::npos && !gone.recorded,
              by_member.reason + " / " + denounced.reason + " / " + after.reason);
    for (const char* then : {"kept", "returned"}) {
      State k = founded();
      Spec x = plain;
      x.then = then, x.notice = 0, x.retention = 3;
      ch::step(k, agreeing("ana", "principal", x, 1));
      ch::step(k, agreeing("sam", "community", x, 1));
      ch::step(k, act("sam", "denounce", 2, {{"agreement", x.id}, {"for", "community"}}));
      const auto principal = ch::step(k, consulting("ana", x.holding, "review", 3)), steward = ch::step(k, consulting("sam", x.holding, "review", 3)),
                 late = ch::step(k, consulting("ana", x.holding, "review", 5));
      s.require(std::string("an agreement denounced for the community with what was recorded ") + then + ": the principal consults it within its retention and not after it, the "
                    "stewards " + (std::string(then) == "kept" ? "too" : "no longer"),
                principal.admitted && steward.admitted == (std::string(then) == "kept") && !late.admitted && late.reason.find("retention") != std::string::npos,
                principal.reason + " / " + steward.reason + " / " + late.reason);
    }
    State m = founded();
    Spec only = plain;
    only.by = "community";
    ch::step(m, agreeing("ana", "principal", only, 1));
    ch::step(m, agreeing("sam", "community", only, 1));
    const auto refused = ch::step(m, act("ana", "denounce", 2, {{"agreement", only.id}, {"for", "principal"}}));
    s.require("an agreement denounced by the community alone refuses the principal's denunciation", !refused.admitted && refused.reason.find("community alone") != std::string::npos, refused.reason);
  }

  // ---- 5. Proposition 24 on random runs --------------------------------------------------------------------------------------------------
  {
    calx::random::Stream r(20261127, 10, 0, 0);
    int holds = 0, agree_with_oracle = 0, never_content = 0, recorded = 0, unrecorded = 0;
    const std::vector<std::string> agents = {"ana/scribe", "ana/clerk", "zed/agent"};
    const std::vector<std::string> kinds = {"reasoning", "plan", "deliberation", "model call"};
    const std::vector<std::string> matters = {"the catalogue", "the bench"};
    for (int trial = 0; trial < 100; trial++) {
      State x = founded();
      ch::Conduct c;
      std::vector<Spec> specs;
      for (int k = 0; k < 2; k++) {
        Spec g;
        g.id = "agreement " + std::to_string(k), g.holding = "record " + std::to_string(k);
        g.principal = k == 0 ? "ana" : "zed";
        g.agents = r.below(2) ? "[]" : (k == 0 ? R"(["ana/scribe"])" : R"(["zed/agent"])");
        g.kinds = "[\"" + kinds[r.below(4)] + "\", \"" + kinds[r.below(4)] + "\"]";
        g.matters = r.below(2) ? "[]" : "[\"" + matters[r.below(2)] + "\"]";
        g.from = between(r, 0, 5), g.to = r.below(2) ? -1 : between(r, g.from + 1, 25), g.ceiling = r.below(2) ? -1 : between(r, 20, 120), g.notice = between(r, 0, 3);
        specs.push_back(g);
      }
      // Ratifications at random turns, perhaps a denunciation, and epistemic episodes between, all in the order of their turns.
      struct Ev {
        long long turn;
        int what, which;
      };
      std::vector<Ev> evs;
      for (int k = 0; k < 2; k++) {
        if (r.below(4) != 0) evs.push_back({between(r, 1, 8), 0, k});
        if (r.below(4) != 0) evs.push_back({between(r, 1, 8), 1, k});
        if (r.below(3) == 0) evs.push_back({between(r, 6, 20), 2, k});
      }
      for (int k = 0; k < 30; k++) evs.push_back({between(r, 0, 30), 3, k});
      std::stable_sort(evs.begin(), evs.end(), [](const Ev& a, const Ev& b) { return a.turn < b.turn; });
      std::map<std::string, long long> used;  // the oracle's reckoning of what each agreement holds
      bool oracle_ok = true, contents_absent = true;
      for (const Ev& ev : evs) {
        if (ev.what == 0) ch::step(x, agreeing(specs[ev.which].principal, "principal", specs[ev.which], ev.turn));
        else if (ev.what == 1) ch::step(x, agreeing("sam", "community", specs[ev.which], ev.turn));
        else if (ev.what == 2) ch::step(x, act("sam", "denounce", ev.turn, {{"agreement", specs[ev.which].id}, {"for", "community"}}));
        else {
          const std::string party = agents[r.below(3)], kind = kinds[r.below(4)], matter = matters[r.below(2)];
          const long long size = between(r, 1, 40);
          const std::string content = "content " + std::to_string(trial) + "." + std::to_string(ev.which) + " of " + party;
          // The oracle, written apart from the fold: the first agreement (in identifier order) both sides ratified by this turn, not ended,
          // whose scope covers the episode and whose ceiling has room.
          std::string expect;
          for (const Spec& g : specs) {
            const auto it = x.agreements.find(g.id);
            if (it == x.agreements.end() || it->second.principal < 0 || it->second.community < 0) continue;
            const long long since = std::max(it->second.principal, it->second.community);
            long long end = g.to >= 0 ? g.to + 1 : -1;
            if (it->second.denounced >= 0) end = end < 0 ? it->second.denounced + g.notice : std::min(end, it->second.denounced + g.notice);
            if (ev.turn < since || (end >= 0 && ev.turn >= end) || ev.turn < g.from || (g.to >= 0 && ev.turn > g.to)) continue;
            const bool own = g.agents == "[]" ? party.rfind(g.principal + "/", 0) == 0 : g.agents.find("\"" + party + "\"") != std::string::npos;
            if (!own || g.kinds.find("\"" + kind + "\"") == std::string::npos || (g.matters != "[]" && g.matters.find("\"" + matter + "\"") == std::string::npos)) continue;
            if (g.ceiling >= 0 && used[g.id] + size > g.ceiling) continue;
            expect = g.id;
            break;
          }
          const auto d = ch::disclose(x, c, thought(party, kind, ev.turn, content, size, matter), chronicle::detail::padded(ev.turn));
          if (d.agreement != expect) oracle_ok = false;
          if (d.recorded) used[d.agreement] += size, recorded++;
          else unrecorded++;
          for (const auto& e : c.journal.entries) contents_absent = contents_absent && e.about.find(content) == std::string::npos;
        }
      }
      holds += ch::disclosed_by_agreement(x, c).holds();
      agree_with_oracle += oracle_ok;
      never_content += contents_absent;
    }
    s.require("on 100 random runs (" + std::to_string(recorded) + " episodes disclosed, " + std::to_string(unrecorded) + " recorded only as reasoning), the journal keeps "
                  "epistemic content only under agreements in force that cover it, every disclosure where an independent reckoning of the terms puts it, and no content in any "
                  "entry (Proposition 24)",
              holds == 100 && agree_with_oracle == 100 && never_content == 100 && recorded > 100 && unrecorded > 100);
  }

  // ---- 6. Proposition 25 on random consults --------------------------------------------------------------------------------------------
  {
    calx::random::Stream r(20261127, 11, 0, 0);
    int agree = 0, total = 0, admitted = 0;
    const std::vector<std::string> who = {"ana", "sam", "rae", "cy", "zed"};
    const std::vector<std::string> uses = {"review", "good faith", "intent", "research", "dispute", ""};
    const std::vector<std::string> thens = {"kept", "returned", "erased"};
    for (int trial = 0; trial < 30; trial++) {
      State x = founded();
      Spec g = plain;
      g.then = thens[r.below(3)], g.notice = between(r, 0, 2), g.retention = between(r, 0, 4), g.principal_reads = r.below(4) != 0;
      g.body = r.below(3) == 0 ? "" : (r.below(2) ? "stewards" : "assembly");
      if (!g.principal_reads && g.body.empty()) g.body = "stewards";
      g.admits = r.below(2) ? R"(["review", "good faith"])" : R"(["review", "research", "dispute"])";
      g.never = r.below(2) ? R"(["intent"])" : "[]";
      ch::step(x, agreeing("ana", "principal", g, 1));
      ch::step(x, agreeing("sam", "community", g, 1));
      const long long denounce = r.below(2) ? between(r, 2, 6) : -1;
      for (long long turn = 2; turn <= 12; turn++) {
        if (turn == denounce) ch::step(x, act("sam", "denounce", turn, {{"agreement", g.id}, {"for", "community"}}));
        const std::string party = who[r.below(who.size())], use = uses[r.below(uses.size())];
        // The independent reading: the parties the terms let consult, for a use admitted and not forbidden, within the phase the end leaves.
        const bool principal = party == "ana";
        const bool positioned = (g.body == "stewards" && party == "sam") || (g.body == "assembly" && (party == "sam" || party == "rae" || party == "cy"));
        const bool admits = !use.empty() && g.admits.find("\"" + use + "\"") != std::string::npos && g.never.find("\"" + use + "\"") == std::string::npos;
        bool expect = admits && ((principal && g.principal_reads) || positioned);
        if (denounce >= 0 && turn >= denounce + g.notice) {
          const long long end = denounce + g.notice;
          if (g.then == "erased" || turn >= end + g.retention || (g.then == "returned" && !principal)) expect = false;
        }
        const auto o = ch::step(x, consulting(party, g.holding, use, turn));
        agree += o.admitted == expect, total++, admitted += o.admitted;
      }
    }
    s.require("on " + std::to_string(total) + " random consults of random agreements (" + std::to_string(admitted) + " admitted), the fold decides as an independent reading of the "
                  "terms does: who may consult, for which uses, before and after the agreement ends (Proposition 25)",
              agree == total && admitted > 20 && admitted < total);
  }

  // ---- 7. Replay, and journals that differ only in what they disclose ----------------------------------------------------------------
  {
    const auto v = ch::replay(parties(), host(), st.acts, st.journal);
    State x = founded(), y = founded();
    ch::Conduct cx, cy;
    for (State* z : {&x, &y}) {
      ch::step(*z, agreeing("ana", "principal", plain, 1));
      ch::step(*z, agreeing("sam", "community", plain, 1));
    }
    ch::disclose(x, cx, thought("ana/scribe", "reasoning", 2, "one thought"), "000000000002");
    ch::disclose(y, cy, thought("ana/scribe", "reasoning", 2, "another thought, longer"), "000000000002");
    ch::disclose(y, cy, thought("ana/scribe", "plan", 2, "a plan undisclosed"), "000000000002");
    s.require("a record with agreements, denunciations and uses replays as sealed; and two journals that differ only in their epistemic entries leave one relational state (G9)",
              v.holds() && ch::relational_digest(x) == ch::relational_digest(y) && cx.journal.head() != cy.journal.head(), v.reason);
  }

  return s.done();
}
