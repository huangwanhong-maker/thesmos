// Checks of the schema of the ground (schema/ground.cat) by kategoria: every statement is answered as the script expects, "fails"
// where the line above says so and "holds" elsewhere. So the worked record satisfies the rules of answerability, and a record
// that makes an agent its own debtor, roots a chain in an operative, lets a stranger act under a mandate, or enters a crossing
// act on one side only, is found by the equation it breaks.

#include <kategoria/script/session.hpp>

#include <calx/verification/check.hpp>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

using kategoria::script::Session;
using kategoria::script::Value;

namespace {
std::string slurp(const std::string& path) {
  std::ifstream in(path);
  std::stringstream s;
  s << in.rdbuf();
  return s.str();
}
std::string field(const Value& o, const char* key) {
  const Value* v = o.get(key);
  return v ? v->text() : std::string();
}
}  // namespace

int main() {
  calx::verification::Suite s;
  const std::string script = slurp(std::string(THESMOS_DATA) + "/schema/ground.cat");
  s.require("the schema's script is read", !script.empty());
  std::vector<std::string> lines;
  {
    std::istringstream in(script);
    for (std::string l; std::getline(in, l);) lines.push_back(l);
  }
  Session session;
  const Value outcomes = session.run(script);
  std::size_t wrong = 0, failing = 0;
  std::string first;
  for (const Value& o : outcomes.items()) {
    const auto line = static_cast<std::size_t>(o.get("line")->number());
    const bool expect_fails = line >= 2 && lines[line - 2].find("# expect fails") == 0;
    failing += expect_fails;
    if (field(o, "status") != (expect_fails ? "fails" : "holds")) {
      wrong++;
      if (first.empty()) first = "line " + std::to_string(line) + ": " + field(o, "status") + ", " + field(o, "reason");
    }
  }
  s.require("kategoria answers every statement of the schema as the script expects (" + std::to_string(outcomes.size()) + " statements, " + std::to_string(failing) + " meant to fail)",
            outcomes.size() >= 6 && failing == 4 && wrong == 0, first);
  return s.done();
}
