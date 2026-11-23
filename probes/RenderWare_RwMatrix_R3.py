# Round 3 for src/RenderWare/RwMatrix.c: the normal-error measure (fn_8047E7A8) and the orthogonality one.
SYMBOLS = {"fn_8047E7A8": "f32 fn_8047E7A8(const RwMatrix *matrix) {",
           "fn_8047E750": "f32 fn_8047E750(const RwMatrix *matrix) {"}
BODIES = {"fn_8047E7A8": {
 "dot": """
    f32 upError = matrix->up.x * matrix->up.x + matrix->up.y * matrix->up.y + matrix->up.z * matrix->up.z - 1.0f;
    f32 rightError = matrix->right.x * matrix->right.x + matrix->right.y * matrix->right.y + matrix->right.z * matrix->right.z - 1.0f;
    f32 atError = matrix->at.x * matrix->at.x + matrix->at.y * matrix->at.y + matrix->at.z * matrix->at.z - 1.0f;

    return rightError * rightError + upError * upError + atError * atError;
""",
 "one_named": """
    f32 rightError;
    RwV3d at;
    RwV3d right;
    RwV3d up;
    f32 atError;
    f32 upError;
    f32 one = 1.0f;

    up.x = matrix->up.x * matrix->up.x;
    up.y = matrix->up.y * matrix->up.y;
    right.x = matrix->right.x * matrix->right.x;
    right.y = matrix->right.y * matrix->right.y;
    up.z = matrix->up.z * matrix->up.z;
    right.z = matrix->right.z * matrix->right.z;
    at.x = matrix->at.x * matrix->at.x;
    at.y = matrix->at.y * matrix->at.y;
    at.z = matrix->at.z * matrix->at.z;
    upError = up.x + up.y + up.z - one;
    rightError = right.x + right.y + right.z - one;
    atError = at.x + at.y + at.z - one;
    return rightError * rightError + upError * upError + atError * atError;
""",
 "sq": """
    RwV3d up;
    RwV3d right;
    RwV3d at;
    f32 upError;
    f32 rightError;
    f32 atError;

    up.x = matrix->up.x * matrix->up.x;
    up.y = matrix->up.y * matrix->up.y;
    up.z = matrix->up.z * matrix->up.z;
    right.x = matrix->right.x * matrix->right.x;
    right.y = matrix->right.y * matrix->right.y;
    right.z = matrix->right.z * matrix->right.z;
    at.x = matrix->at.x * matrix->at.x;
    at.y = matrix->at.y * matrix->at.y;
    at.z = matrix->at.z * matrix->at.z;
    upError = up.x + up.y + up.z - 1.0f;
    rightError = right.x + right.y + right.z - 1.0f;
    atError = at.x + at.y + at.z - 1.0f;
    return rightError * rightError + upError * upError + atError * atError;
""",
 "sum3": """
    f32 upError = matrix->up.x * matrix->up.x + matrix->up.y * matrix->up.y;
    f32 rightError = matrix->right.x * matrix->right.x + matrix->right.y * matrix->right.y;
    f32 atError;

    upError = matrix->up.z * matrix->up.z + upError - 1.0f;
    rightError = matrix->right.z * matrix->right.z + rightError - 1.0f;
    atError = matrix->at.x * matrix->at.x + matrix->at.y * matrix->at.y;
    atError = matrix->at.z * matrix->at.z + atError - 1.0f;
    return rightError * rightError + upError * upError + atError * atError;
""",
},
"fn_8047E750": {
 "orth_a": """
    f32 upAt = matrix->up.x * matrix->at.x + matrix->up.y * matrix->at.y + matrix->up.z * matrix->at.z;
    f32 atRight = matrix->at.x * matrix->right.x + matrix->at.y * matrix->right.y + matrix->at.z * matrix->right.z;
    f32 rightUp = matrix->right.x * matrix->up.x + matrix->right.y * matrix->up.y + matrix->right.z * matrix->up.z;

    return upAt * upAt + atRight * atRight + rightUp * rightUp;
""",
 "orth_b": """
    f32 rightUp = matrix->right.x * matrix->up.x + matrix->right.y * matrix->up.y + matrix->right.z * matrix->up.z;
    f32 upAt = matrix->up.x * matrix->at.x + matrix->up.y * matrix->at.y + matrix->up.z * matrix->at.z;
    f32 atRight = matrix->at.x * matrix->right.x + matrix->at.y * matrix->right.y + matrix->at.z * matrix->right.z;

    return upAt * upAt + atRight * atRight + rightUp * rightUp;
""",
 "orth_c": """
    f32 atRight = matrix->at.x * matrix->right.x + matrix->at.y * matrix->right.y + matrix->at.z * matrix->right.z;
    f32 upAt = matrix->up.x * matrix->at.x + matrix->up.y * matrix->at.y + matrix->up.z * matrix->at.z;
    f32 rightUp = matrix->right.x * matrix->up.x + matrix->right.y * matrix->up.y + matrix->right.z * matrix->up.z;

    return upAt * upAt + atRight * atRight + rightUp * rightUp;
""",
 "orth_d": """
    f32 upAt = matrix->at.x * matrix->up.x + matrix->at.y * matrix->up.y + matrix->at.z * matrix->up.z;
    f32 atRight = matrix->right.x * matrix->at.x + matrix->right.y * matrix->at.y + matrix->right.z * matrix->at.z;
    f32 rightUp = matrix->up.x * matrix->right.x + matrix->up.y * matrix->right.y + matrix->up.z * matrix->right.z;

    return upAt * upAt + atRight * atRight + rightUp * rightUp;
""",
}}
