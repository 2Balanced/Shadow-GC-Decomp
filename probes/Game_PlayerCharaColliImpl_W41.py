"""Wave 41: the list range constructor's argument copies (retail: last's copy above first's). The variants switched
between temporary MSL_LRV1..5 code paths added to <list> for the run and removed afterwards, so this
table is a record only (all five kept 6 rows or more)."""
SCORE = ["__ct<Q23std96__list_const_iterator<Q25boost63shared_ptr<Q412SonicteamUSA6System10CharaColli14CharaColliBase>>>__Q23std86__list_base<Q25boost63shared_ptr<Q412SonicteamUSA6System10CharaColli14CharaColliBase>>FQ23std96__list_const_iterator<Q25boost63shared_ptr<Q412SonicteamUSA6System10CharaColli14CharaColliBase>>Q23std96__list_const_iterator<Q25boost63shared_ptr<Q412SonicteamUSA6System10CharaColli14CharaColliBase>>RCQ23std97__list_value_allocator<Q25boost63shared_ptr<Q412SonicteamUSA6System10CharaColli14CharaColliBase>>"]
D = "#define MSL_LIST_RANGE_INSERT\n"
REPL = {n: [(D, D + "#define MSL_%s\n" % n.upper())] for n in ["lrv1", "lrv2", "lrv3", "lrv4", "lrv5"]}
