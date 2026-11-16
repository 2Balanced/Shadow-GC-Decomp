"""ScatteringRing::Instantiate (wave 39, second pass): the request's count is loaded after the
direction's stores, which GC/2.7 only keeps for a request that is not const."""
R = "Instantiate__Q214ScatteringRing14ScatteringRingFRC26UnkScatterRequest_800B7E40b"
SCORE = [(R, "Instantiate__Q214ScatteringRing14ScatteringRingFR26UnkScatterRequest_800B7E40b")]
A = "static void Instantiate(const UnkScatterRequest_800B7E40 &request, bool flag);"
B = "void ScatteringRing::Instantiate(const UnkScatterRequest_800B7E40 &request, bool flag) {"
REPL = {"nonconst": [(A, A.replace("const ", "")), (B, B.replace("const ", ""))]}
