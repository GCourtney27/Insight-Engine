#if 0
//
// Generated by Microsoft (R) D3D Shader Disassembler
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_Position              0   xyzw        0     NONE   float   xyzw
// NORMAL                   0   xyz         1     NONE   float   xyz 
// TEXCOORD                 0   xy          2     NONE   float   xy  
// BLENDINDICES             0   xyzw        3     NONE    uint   x   
// BLENDWEIGHT              0   xyzw        4     NONE   float   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// TEXCOORD                 0   xy          0     NONE   float   xy  
// TEXCOORD                 1   xyzw        1     NONE   float   xyzw
// TEXCOORD                 2   xyz         2     NONE   float   xyz 
// COLOR                    0   xyzw        3     NONE   float   xyzw
// SV_Position              0   xyzw        4      POS   float   xyzw
//
vs_5_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[0:0][242], dynamicIndexed, space=0
dcl_input v0.xyzw
dcl_input v1.xyz
dcl_input v2.xy
dcl_input v3.x
dcl_input v4.x
dcl_output o0.xy
dcl_output o1.xyzw
dcl_output o2.xyz
dcl_output o3.xyzw
dcl_output_siv o4.xyzw, position
dcl_temps 4
imul null, r0.x, v3.x, l(3)
mul r1.xyzw, v4.xxxx, CB0[0][r0.x + 26].xyzw
mul r2.xyzw, v4.xxxx, CB0[0][r0.x + 27].xyzw
mul r0.xyzw, v4.xxxx, CB0[0][r0.x + 28].xyzw
dp4 r3.x, v0.xyzw, r1.xyzw
dp4 r3.y, v0.xyzw, r2.xyzw
dp4 r3.z, v0.xyzw, r0.xyzw
dp3 r1.x, v1.xyzx, r1.xyzx
dp3 r1.y, v1.xyzx, r2.xyzx
dp3 r1.z, v1.xyzx, r0.xyzx
mov r3.w, v0.w
dp4 o4.x, r3.xyzw, CB0[0][22].xyzw
dp4 o4.y, r3.xyzw, CB0[0][23].xyzw
dp4 o4.z, r3.xyzw, CB0[0][24].xyzw
dp4 o4.w, r3.xyzw, CB0[0][25].xyzw
dp4 o1.x, r3.xyzw, CB0[0][15].xyzw
dp4 o1.y, r3.xyzw, CB0[0][16].xyzw
dp4 o1.z, r3.xyzw, CB0[0][17].xyzw
dp3 r0.x, r1.xyzx, CB0[0][19].xyzx
dp3 r0.y, r1.xyzx, CB0[0][20].xyzx
dp3 r0.z, r1.xyzx, CB0[0][21].xyzx
dp3 r0.w, r0.xyzx, r0.xyzx
rsq r0.w, r0.w
mul o2.xyz, r0.wwww, r0.xyzx
dp4_sat o1.w, r3.xyzw, CB0[0][14].xyzw
mov o3.xyz, l(1.000000,1.000000,1.000000,0)
mov o3.w, CB0[0][0].w
mov o0.xy, v2.xyxx
ret 
// Approximately 0 instruction slots used
#endif

const BYTE SkinnedEffect_VSSkinnedPixelLightingOneBone[] =
{
     68,  88,  66,  67,  24, 197, 
     26, 206, 243,  34, 187, 134, 
    222,  62, 121, 188, 185, 252, 
    109, 202,   1,   0,   0,   0, 
    100,   6,   0,   0,   4,   0, 
      0,   0,  48,   0,   0,   0, 
    240,   0,   0,   0, 148,   1, 
      0,   0, 212,   5,   0,   0, 
     73,  83,  71,  78, 184,   0, 
      0,   0,   5,   0,   0,   0, 
      8,   0,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,  15,   0,   0, 140,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      7,   7,   0,   0, 147,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      3,   3,   0,   0, 156,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   1,   0,   0, 169,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
     15,   1,   0,   0,  83,  86, 
     95,  80, 111, 115, 105, 116, 
    105, 111, 110,   0,  78,  79, 
     82,  77,  65,  76,   0,  84, 
     69,  88,  67,  79,  79,  82, 
     68,   0,  66,  76,  69,  78, 
     68,  73,  78,  68,  73,  67, 
     69,  83,   0,  66,  76,  69, 
     78,  68,  87,  69,  73,  71, 
     72,  84,   0, 171, 171, 171, 
     79,  83,  71,  78, 156,   0, 
      0,   0,   5,   0,   0,   0, 
      8,   0,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,  12,   0,   0, 128,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     15,   0,   0,   0, 128,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
      7,   8,   0,   0, 137,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   0,   0,   0, 143,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   4,   0,   0,   0, 
     15,   0,   0,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  67,  79,  76,  79,  82, 
      0,  83,  86,  95,  80, 111, 
    115, 105, 116, 105, 111, 110, 
      0, 171,  83,  72,  69,  88, 
     56,   4,   0,   0,  81,   0, 
      1,   0,  14,   1,   0,   0, 
    106,   8,   0,   1,  89,   8, 
      0,   7,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    242,   0,   0,   0,   0,   0, 
      0,   0,  95,   0,   0,   3, 
    242,  16,  16,   0,   0,   0, 
      0,   0,  95,   0,   0,   3, 
    114,  16,  16,   0,   1,   0, 
      0,   0,  95,   0,   0,   3, 
     50,  16,  16,   0,   2,   0, 
      0,   0,  95,   0,   0,   3, 
     18,  16,  16,   0,   3,   0, 
      0,   0,  95,   0,   0,   3, 
     18,  16,  16,   0,   4,   0, 
      0,   0, 101,   0,   0,   3, 
     50,  32,  16,   0,   0,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   1,   0, 
      0,   0, 101,   0,   0,   3, 
    114,  32,  16,   0,   2,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   3,   0, 
      0,   0, 103,   0,   0,   4, 
    242,  32,  16,   0,   4,   0, 
      0,   0,   1,   0,   0,   0, 
    104,   0,   0,   2,   4,   0, 
      0,   0,  38,   0,   0,   8, 
      0, 208,   0,   0,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     10,  16,  16,   0,   3,   0, 
      0,   0,   1,  64,   0,   0, 
      3,   0,   0,   0,  56,   0, 
      0,  11, 242,   0,  16,   0, 
      1,   0,   0,   0,   6,  16, 
     16,   0,   4,   0,   0,   0, 
     70, 142,  48,  48,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     26,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,  11, 242,   0, 
     16,   0,   2,   0,   0,   0, 
      6,  16,  16,   0,   4,   0, 
      0,   0,  70, 142,  48,  48, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  27,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  56,   0,   0,  11, 
    242,   0,  16,   0,   0,   0, 
      0,   0,   6,  16,  16,   0, 
      4,   0,   0,   0,  70, 142, 
     48,  48,   0,   0,   0,   0, 
      0,   0,   0,   0,  28,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  17,   0, 
      0,   7,  18,   0,  16,   0, 
      3,   0,   0,   0,  70,  30, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  17,   0,   0,   7, 
     34,   0,  16,   0,   3,   0, 
      0,   0,  70,  30,  16,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   2,   0,   0,   0, 
     17,   0,   0,   7,  66,   0, 
     16,   0,   3,   0,   0,   0, 
     70,  30,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  70,  18, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  16,   0,   0,   7, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  70,  18,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     16,   0,   0,   7,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  18,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      3,   0,   0,   0,  58,  16, 
     16,   0,   0,   0,   0,   0, 
     17,   0,   0,   9,  18,  32, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  22,   0,   0,   0, 
     17,   0,   0,   9,  34,  32, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  23,   0,   0,   0, 
     17,   0,   0,   9,  66,  32, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  24,   0,   0,   0, 
     17,   0,   0,   9, 130,  32, 
     16,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  25,   0,   0,   0, 
     17,   0,   0,   9,  18,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     17,   0,   0,   9,  34,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  16,   0,   0,   0, 
     17,   0,   0,   9,  66,  32, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   3,   0, 
      0,   0,  70, 142,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  17,   0,   0,   0, 
     16,   0,   0,   9,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70, 130,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  19,   0,   0,   0, 
     16,   0,   0,   9,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70, 130,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  20,   0,   0,   0, 
     16,   0,   0,   9,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70, 130,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  21,   0,   0,   0, 
     16,   0,   0,   7, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  68,   0, 
      0,   5, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 114,  32, 
     16,   0,   2,   0,   0,   0, 
    246,  15,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  17,  32, 
      0,   9, 130,  32,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     70, 142,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     14,   0,   0,   0,  54,   0, 
      0,   8, 114,  32,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0,   0,   0, 
     54,   0,   0,   7, 130,  32, 
     16,   0,   3,   0,   0,   0, 
     58, 128,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5,  50,  32,  16,   0, 
      0,   0,   0,   0,  70,  16, 
     16,   0,   2,   0,   0,   0, 
     62,   0,   0,   1,  82,  84, 
     83,  48, 136,   0,   0,   0, 
      2,   0,   0,   0,   3,   0, 
      0,   0,  24,   0,   0,   0, 
      0,   0,   0,   0, 136,   0, 
      0,   0,  29,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,  60,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  72,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0, 104,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,  80,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   1,   0,   0,   0, 
    112,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255
};