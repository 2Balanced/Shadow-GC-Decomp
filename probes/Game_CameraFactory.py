"""Wave 40 shapes for fn_8000CD64 (Game/CameraFactory.cpp): the parent (r4) and the grandparent
(r5) swap registers at the loop's top.  All of these are spellings of the Parent() helper."""
P = """inline TreeNode *Parent(const TreeNode *node) {
    return (TreeNode *)(node->parent_ & ~1);
}"""
C1 = "            if (Parent(x) == Parent(Parent(x))->left_) {\n                T *y = Parent(Parent(x))->right_;"
SCORE = ["fn_8000CD64"]
REPL = {
    "base": [(P, P)],
    "nonconst": [(P, P.replace("const TreeNode *node", "TreeNode *node"))],
    "macro": [(P, "#define Parent(n) ((TreeNode *)((n)->parent_ & ~1))")],
    "tmpl": [(P, "template <class N> inline N *Parent(N *node) {\n    return (N *)(node->parent_ & ~1);\n}")],
    "ref": [(P, "inline TreeNode *ParentR(const TreeNode &node) {\n    return (TreeNode *)(node.parent_ & ~1);\n}\n#define Parent(n) ParentR(*(n))")],
    "local": [(P, "inline TreeNode *Parent(const TreeNode *node) {\n    TreeNode *p = (TreeNode *)(node->parent_ & ~1);\n    return p;\n}")],
    "ulong": [(P, "inline unsigned long ParentU(const TreeNode *node) {\n    return node->parent_ & ~1;\n}\n#define Parent(n) ((TreeNode *)ParentU(n))")],
    "getter": [(P, "inline unsigned long ParentBits(const TreeNode *node) {\n    return node->parent_;\n}\ninline TreeNode *Parent(const TreeNode *node) {\n    return (TreeNode *)(ParentBits(node) & ~1);\n}")],
    "grand": [(P, P + "\ninline TreeNode *Grand(const TreeNode *node) {\n    return (TreeNode *)(((TreeNode *)(node->parent_ & ~1))->parent_ & ~1);\n}"),
              (C1, C1.replace("Parent(Parent(x))", "Grand(x)"))],
    "grand_cmp": [(P, P + "\ninline TreeNode *Grand(const TreeNode *node) {\n    return (TreeNode *)(((TreeNode *)(node->parent_ & ~1))->parent_ & ~1);\n}"),
              (C1, "            if (Parent(x) == Grand(x)->left_) {\n                T *y = Parent(Parent(x))->right_;")],
    "left_helper": [(P, P + "\ninline TreeNode *Left(const TreeNode *node) {\n    return node->left_;\n}"),
              (C1, "            if (Parent(x) == Left(Parent(Parent(x)))) {\n                T *y = Parent(Parent(x))->right_;")],
    "isleft": [(P, P + "\ninline bool IsLeft(const TreeNode *node, const TreeNode *g) {\n    return node == g->left_;\n}"),
              (C1, "            if (IsLeft(Parent(x), Parent(Parent(x)))) {\n                T *y = Parent(Parent(x))->right_;")],
    "isleft2": [(P, P + "\ninline bool IsLeft(const TreeNode *g, const TreeNode *node) {\n    return node == g->left_;\n}"),
              (C1, "            if (IsLeft(Parent(Parent(x)), Parent(x))) {\n                T *y = Parent(Parent(x))->right_;")],
    "ternary_y": [(C1, "            T *y;\n            if (Parent(x) == Parent(Parent(x))->left_) {\n                y = Parent(Parent(x))->right_;"),
                  ("                T *y = Parent(Parent(x))->left_;", "                y = Parent(Parent(x))->left_;")],
}
