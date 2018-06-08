// Created by http://oleddisplay.squix.ch/ Consider a donation
// In case of problems make sure that you are using the font file with the correct version!
const uint8_t Roboto_12[] PROGMEM = {
	0x0B, // Width: 11
	0x0F, // Height: 15
	0x20, // First Char: 32
	0xE0, // Numbers of Chars: 224

	// Jump Table:
	0xFF, 0xFF, 0x00, 0x03,  // 32:65535
	0x00, 0x00, 0x04, 0x03,  // 33:0
	0x00, 0x04, 0x05, 0x04,  // 34:4
	0x00, 0x09, 0x0E, 0x07,  // 35:9
	0x00, 0x17, 0x0C, 0x07,  // 36:23
	0x00, 0x23, 0x10, 0x09,  // 37:35
	0x00, 0x33, 0x0E, 0x07,  // 38:51
	0x00, 0x41, 0x01, 0x02,  // 39:65
	0x00, 0x42, 0x08, 0x04,  // 40:66
	0x00, 0x4A, 0x06, 0x04,  // 41:74
	0x00, 0x50, 0x09, 0x05,  // 42:80
	0x00, 0x59, 0x0B, 0x07,  // 43:89
	0x00, 0x64, 0x04, 0x02,  // 44:100
	0x00, 0x68, 0x06, 0x03,  // 45:104
	0x00, 0x6E, 0x04, 0x03,  // 46:110
	0x00, 0x72, 0x09, 0x05,  // 47:114
	0x00, 0x7B, 0x0C, 0x07,  // 48:123
	0x00, 0x87, 0x08, 0x07,  // 49:135
	0x00, 0x8F, 0x0C, 0x07,  // 50:143
	0x00, 0x9B, 0x0C, 0x07,  // 51:155
	0x00, 0xA7, 0x0C, 0x07,  // 52:167
	0x00, 0xB3, 0x0C, 0x07,  // 53:179
	0x00, 0xBF, 0x0C, 0x07,  // 54:191
	0x00, 0xCB, 0x0B, 0x07,  // 55:203
	0x00, 0xD6, 0x0C, 0x07,  // 56:214
	0x00, 0xE2, 0x0C, 0x07,  // 57:226
	0x00, 0xEE, 0x04, 0x03,  // 58:238
	0x00, 0xF2, 0x04, 0x03,  // 59:242
	0x00, 0xF6, 0x0A, 0x06,  // 60:246
	0x01, 0x00, 0x0C, 0x07,  // 61:256
	0x01, 0x0C, 0x0B, 0x06,  // 62:268
	0x01, 0x17, 0x09, 0x06,  // 63:279
	0x01, 0x20, 0x14, 0x0B,  // 64:288
	0x01, 0x34, 0x10, 0x08,  // 65:308
	0x01, 0x44, 0x0E, 0x07,  // 66:324
	0x01, 0x52, 0x0E, 0x08,  // 67:338
	0x01, 0x60, 0x0E, 0x08,  // 68:352
	0x01, 0x6E, 0x0C, 0x07,  // 69:366
	0x01, 0x7A, 0x0B, 0x07,  // 70:378
	0x01, 0x85, 0x0E, 0x08,  // 71:389
	0x01, 0x93, 0x10, 0x09,  // 72:403
	0x01, 0xA3, 0x04, 0x03,  // 73:419
	0x01, 0xA7, 0x0C, 0x07,  // 74:423
	0x01, 0xB3, 0x10, 0x08,  // 75:435
	0x01, 0xC3, 0x0C, 0x06,  // 76:451
	0x01, 0xCF, 0x12, 0x0A,  // 77:463
	0x01, 0xE1, 0x10, 0x09,  // 78:481
	0x01, 0xF1, 0x10, 0x08,  // 79:497
	0x02, 0x01, 0x0D, 0x08,  // 80:513
	0x02, 0x0E, 0x0E, 0x08,  // 81:526
	0x02, 0x1C, 0x0E, 0x07,  // 82:540
	0x02, 0x2A, 0x0E, 0x07,  // 83:554
	0x02, 0x38, 0x0D, 0x07,  // 84:568
	0x02, 0x45, 0x0E, 0x08,  // 85:581
	0x02, 0x53, 0x0F, 0x08,  // 86:595
	0x02, 0x62, 0x13, 0x0B,  // 87:610
	0x02, 0x75, 0x0E, 0x08,  // 88:629
	0x02, 0x83, 0x0D, 0x07,  // 89:643
	0x02, 0x90, 0x0E, 0x07,  // 90:656
	0x02, 0x9E, 0x06, 0x03,  // 91:670
	0x02, 0xA4, 0x0A, 0x05,  // 92:676
	0x02, 0xAE, 0x04, 0x03,  // 93:686
	0x02, 0xB2, 0x09, 0x05,  // 94:690
	0x02, 0xBB, 0x0A, 0x05,  // 95:699
	0x02, 0xC5, 0x05, 0x04,  // 96:709
	0x02, 0xCA, 0x0C, 0x07,  // 97:714
	0x02, 0xD6, 0x0C, 0x07,  // 98:726
	0x02, 0xE2, 0x0C, 0x06,  // 99:738
	0x02, 0xEE, 0x0C, 0x07,  // 100:750
	0x02, 0xFA, 0x0C, 0x06,  // 101:762
	0x03, 0x06, 0x07, 0x04,  // 102:774
	0x03, 0x0D, 0x0C, 0x07,  // 103:781
	0x03, 0x19, 0x0C, 0x07,  // 104:793
	0x03, 0x25, 0x04, 0x03,  // 105:805
	0x03, 0x29, 0x04, 0x03,  // 106:809
	0x03, 0x2D, 0x0C, 0x06,  // 107:813
	0x03, 0x39, 0x04, 0x03,  // 108:825
	0x03, 0x3D, 0x14, 0x0B,  // 109:829
	0x03, 0x51, 0x0C, 0x07,  // 110:849
	0x03, 0x5D, 0x0C, 0x07,  // 111:861
	0x03, 0x69, 0x0C, 0x07,  // 112:873
	0x03, 0x75, 0x0C, 0x07,  // 113:885
	0x03, 0x81, 0x07, 0x04,  // 114:897
	0x03, 0x88, 0x0A, 0x06,  // 115:904
	0x03, 0x92, 0x08, 0x04,  // 116:914
	0x03, 0x9A, 0x0C, 0x07,  // 117:922
	0x03, 0xA6, 0x0B, 0x06,  // 118:934
	0x03, 0xB1, 0x11, 0x09,  // 119:945
	0x03, 0xC2, 0x0C, 0x06,  // 120:962
	0x03, 0xCE, 0x0B, 0x06,  // 121:974
	0x03, 0xD9, 0x0C, 0x06,  // 122:985
	0x03, 0xE5, 0x08, 0x04,  // 123:997
	0x03, 0xED, 0x04, 0x03,  // 124:1005
	0x03, 0xF1, 0x08, 0x04,  // 125:1009
	0x03, 0xF9, 0x0E, 0x08,  // 126:1017
	0x04, 0x07, 0x0A, 0x05,  // 127:1031
	0x04, 0x11, 0x0A, 0x05,  // 128:1041
	0x04, 0x1B, 0x0A, 0x05,  // 129:1051
	0x04, 0x25, 0x0A, 0x05,  // 130:1061
	0x04, 0x2F, 0x0A, 0x05,  // 131:1071
	0x04, 0x39, 0x0A, 0x05,  // 132:1081
	0x04, 0x43, 0x0A, 0x05,  // 133:1091
	0x04, 0x4D, 0x0A, 0x05,  // 134:1101
	0x04, 0x57, 0x0A, 0x05,  // 135:1111
	0x04, 0x61, 0x0A, 0x05,  // 136:1121
	0x04, 0x6B, 0x0A, 0x05,  // 137:1131
	0x04, 0x75, 0x0A, 0x05,  // 138:1141
	0x04, 0x7F, 0x0A, 0x05,  // 139:1151
	0x04, 0x89, 0x0A, 0x05,  // 140:1161
	0x04, 0x93, 0x0A, 0x05,  // 141:1171
	0x04, 0x9D, 0x0A, 0x05,  // 142:1181
	0x04, 0xA7, 0x0A, 0x05,  // 143:1191
	0x04, 0xB1, 0x0A, 0x05,  // 144:1201
	0x04, 0xBB, 0x0A, 0x05,  // 145:1211
	0x04, 0xC5, 0x0A, 0x05,  // 146:1221
	0x04, 0xCF, 0x0A, 0x05,  // 147:1231
	0x04, 0xD9, 0x0A, 0x05,  // 148:1241
	0x04, 0xE3, 0x0A, 0x05,  // 149:1251
	0x04, 0xED, 0x0A, 0x05,  // 150:1261
	0x04, 0xF7, 0x0A, 0x05,  // 151:1271
	0x05, 0x01, 0x0A, 0x05,  // 152:1281
	0x05, 0x0B, 0x0A, 0x05,  // 153:1291
	0x05, 0x15, 0x0A, 0x05,  // 154:1301
	0x05, 0x1F, 0x0A, 0x05,  // 155:1311
	0x05, 0x29, 0x0A, 0x05,  // 156:1321
	0x05, 0x33, 0x0A, 0x05,  // 157:1331
	0x05, 0x3D, 0x0A, 0x05,  // 158:1341
	0x05, 0x47, 0x0A, 0x05,  // 159:1351
	0xFF, 0xFF, 0x00, 0x03,  // 160:65535
	0x05, 0x51, 0x04, 0x03,  // 161:1361
	0x05, 0x55, 0x0C, 0x07,  // 162:1365
	0x05, 0x61, 0x0E, 0x07,  // 163:1377
	0x05, 0x6F, 0x10, 0x09,  // 164:1391
	0x05, 0x7F, 0x0B, 0x06,  // 165:1407
	0x05, 0x8A, 0x04, 0x03,  // 166:1418
	0x05, 0x8E, 0x0E, 0x07,  // 167:1422
	0x05, 0x9C, 0x07, 0x05,  // 168:1436
	0x05, 0xA3, 0x12, 0x09,  // 169:1443
	0x05, 0xB5, 0x07, 0x05,  // 170:1461
	0x05, 0xBC, 0x0A, 0x06,  // 171:1468
	0x05, 0xC6, 0x0C, 0x07,  // 172:1478
	0x05, 0xD2, 0x06, 0x03,  // 173:1490
	0x05, 0xD8, 0x12, 0x09,  // 174:1496
	0x05, 0xEA, 0x09, 0x06,  // 175:1514
	0x05, 0xF3, 0x07, 0x04,  // 176:1523
	0x05, 0xFA, 0x0C, 0x06,  // 177:1530
	0x06, 0x06, 0x07, 0x04,  // 178:1542
	0x06, 0x0D, 0x07, 0x04,  // 179:1549
	0x06, 0x14, 0x05, 0x04,  // 180:1556
	0x06, 0x19, 0x0C, 0x07,  // 181:1561
	0x06, 0x25, 0x0A, 0x06,  // 182:1573
	0x06, 0x2F, 0x03, 0x03,  // 183:1583
	0x06, 0x32, 0x06, 0x03,  // 184:1586
	0x06, 0x38, 0x05, 0x04,  // 185:1592
	0x06, 0x3D, 0x09, 0x05,  // 186:1597
	0x06, 0x46, 0x0A, 0x06,  // 187:1606
	0x06, 0x50, 0x10, 0x09,  // 188:1616
	0x06, 0x60, 0x12, 0x09,  // 189:1632
	0x06, 0x72, 0x12, 0x09,  // 190:1650
	0x06, 0x84, 0x0A, 0x06,  // 191:1668
	0x06, 0x8E, 0x10, 0x08,  // 192:1678
	0x06, 0x9E, 0x10, 0x08,  // 193:1694
	0x06, 0xAE, 0x10, 0x08,  // 194:1710
	0x06, 0xBE, 0x10, 0x08,  // 195:1726
	0x06, 0xCE, 0x10, 0x08,  // 196:1742
	0x06, 0xDE, 0x10, 0x08,  // 197:1758
	0x06, 0xEE, 0x16, 0x0B,  // 198:1774
	0x07, 0x04, 0x0E, 0x08,  // 199:1796
	0x07, 0x12, 0x0C, 0x07,  // 200:1810
	0x07, 0x1E, 0x0C, 0x07,  // 201:1822
	0x07, 0x2A, 0x0C, 0x07,  // 202:1834
	0x07, 0x36, 0x0C, 0x07,  // 203:1846
	0x07, 0x42, 0x04, 0x03,  // 204:1858
	0x07, 0x46, 0x05, 0x03,  // 205:1862
	0x07, 0x4B, 0x05, 0x03,  // 206:1867
	0x07, 0x50, 0x05, 0x03,  // 207:1872
	0x07, 0x55, 0x0E, 0x08,  // 208:1877
	0x07, 0x63, 0x10, 0x09,  // 209:1891
	0x07, 0x73, 0x10, 0x08,  // 210:1907
	0x07, 0x83, 0x10, 0x08,  // 211:1923
	0x07, 0x93, 0x10, 0x08,  // 212:1939
	0x07, 0xA3, 0x10, 0x08,  // 213:1955
	0x07, 0xB3, 0x10, 0x08,  // 214:1971
	0x07, 0xC3, 0x0C, 0x06,  // 215:1987
	0x07, 0xCF, 0x0F, 0x08,  // 216:1999
	0x07, 0xDE, 0x0E, 0x08,  // 217:2014
	0x07, 0xEC, 0x0E, 0x08,  // 218:2028
	0x07, 0xFA, 0x0E, 0x08,  // 219:2042
	0x08, 0x08, 0x0E, 0x08,  // 220:2056
	0x08, 0x16, 0x0D, 0x07,  // 221:2070
	0x08, 0x23, 0x0D, 0x07,  // 222:2083
	0x08, 0x30, 0x0E, 0x07,  // 223:2096
	0x08, 0x3E, 0x0C, 0x07,  // 224:2110
	0x08, 0x4A, 0x0C, 0x07,  // 225:2122
	0x08, 0x56, 0x0C, 0x07,  // 226:2134
	0x08, 0x62, 0x0C, 0x07,  // 227:2146
	0x08, 0x6E, 0x0C, 0x07,  // 228:2158
	0x08, 0x7A, 0x0C, 0x07,  // 229:2170
	0x08, 0x86, 0x14, 0x0A,  // 230:2182
	0x08, 0x9A, 0x0C, 0x06,  // 231:2202
	0x08, 0xA6, 0x0C, 0x06,  // 232:2214
	0x08, 0xB2, 0x0C, 0x06,  // 233:2226
	0x08, 0xBE, 0x0C, 0x06,  // 234:2238
	0x08, 0xCA, 0x0C, 0x06,  // 235:2250
	0x08, 0xD6, 0x04, 0x03,  // 236:2262
	0x08, 0xDA, 0x05, 0x03,  // 237:2266
	0x08, 0xDF, 0x05, 0x03,  // 238:2271
	0x08, 0xE4, 0x05, 0x03,  // 239:2276
	0x08, 0xE9, 0x0C, 0x07,  // 240:2281
	0x08, 0xF5, 0x0C, 0x07,  // 241:2293
	0x09, 0x01, 0x0C, 0x07,  // 242:2305
	0x09, 0x0D, 0x0C, 0x07,  // 243:2317
	0x09, 0x19, 0x0C, 0x07,  // 244:2329
	0x09, 0x25, 0x0C, 0x07,  // 245:2341
	0x09, 0x31, 0x0C, 0x07,  // 246:2353
	0x09, 0x3D, 0x0B, 0x07,  // 247:2365
	0x09, 0x48, 0x0C, 0x07,  // 248:2376
	0x09, 0x54, 0x0C, 0x07,  // 249:2388
	0x09, 0x60, 0x0C, 0x07,  // 250:2400
	0x09, 0x6C, 0x0C, 0x07,  // 251:2412
	0x09, 0x78, 0x0C, 0x07,  // 252:2424
	0x09, 0x84, 0x0B, 0x06,  // 253:2436
	0x09, 0x8F, 0x0C, 0x07,  // 254:2447
	0x09, 0x9B, 0x0B, 0x06,  // 255:2459

	// Font Data:
	0x00,0x00,0xF8,0x0B,	// 33
	0x00,0x00,0x38,0x00,0x38,	// 34
	0x00,0x00,0x20,0x01,0xE0,0x0F,0x38,0x01,0x20,0x0F,0xF8,0x01,0x28,0x01,	// 35
	0x00,0x00,0x70,0x06,0xC8,0x08,0x8C,0x18,0x88,0x09,0x30,0x07,	// 36
	0x00,0x00,0x78,0x00,0x48,0x04,0x78,0x03,0xC0,0x00,0x20,0x0F,0x10,0x09,0x00,0x0F,	// 37
	0x00,0x00,0x30,0x07,0xD8,0x08,0xC8,0x09,0x78,0x0B,0x00,0x04,0x00,0x0B,	// 38
	0x38,	// 39
	0x00,0x00,0xE0,0x0F,0x18,0x30,0x04,0x40,	// 40
	0x04,0x40,0x18,0x30,0xE0,0x0F,	// 41
	0x10,0x00,0xE0,0x00,0x38,0x00,0xE0,0x00,0x10,	// 42
	0x80,0x00,0x80,0x00,0x80,0x00,0xE0,0x07,0x80,0x00,0x80,	// 43
	0x00,0x20,0x00,0x18,	// 44
	0x00,0x01,0x00,0x01,0x00,0x01,	// 45
	0x00,0x00,0x00,0x08,	// 46
	0x00,0x10,0x00,0x0E,0xC0,0x01,0x38,0x00,0x08,	// 47
	0x00,0x00,0xF0,0x07,0x08,0x08,0x08,0x08,0x18,0x0C,0xF0,0x07,	// 48
	0x00,0x00,0x10,0x00,0x10,0x00,0xF8,0x0F,	// 49
	0x00,0x00,0x30,0x0C,0x08,0x0A,0x08,0x09,0x98,0x08,0x70,0x08,	// 50
	0x00,0x00,0x10,0x04,0x88,0x08,0x88,0x08,0xD8,0x08,0x70,0x07,	// 51
	0x00,0x02,0x80,0x03,0x40,0x02,0x30,0x02,0xF8,0x0F,0x00,0x02,	// 52
	0x00,0x00,0xF0,0x04,0x58,0x08,0x48,0x08,0x48,0x08,0x88,0x07,	// 53
	0x00,0x00,0xE0,0x07,0x50,0x08,0x48,0x08,0x48,0x08,0x80,0x07,	// 54
	0x08,0x00,0x08,0x00,0x08,0x0C,0x08,0x03,0xE8,0x00,0x18,	// 55
	0x00,0x00,0x70,0x07,0x88,0x08,0x88,0x08,0xD8,0x08,0x70,0x07,	// 56
	0x00,0x00,0xF0,0x00,0x08,0x09,0x08,0x09,0x98,0x04,0xF0,0x03,	// 57
	0x00,0x00,0x20,0x08,	// 58
	0x00,0x20,0x20,0x18,	// 59
	0x80,0x00,0x80,0x00,0x40,0x01,0x40,0x01,0x20,0x02,	// 60
	0x00,0x00,0x80,0x02,0x80,0x02,0x80,0x02,0x80,0x02,0x80,0x02,	// 61
	0x00,0x00,0x20,0x02,0x40,0x01,0x40,0x01,0x80,0x01,0x80,	// 62
	0x20,0x00,0x18,0x00,0x08,0x0B,0x88,0x00,0x70,	// 63
	0x00,0x00,0x80,0x1F,0x60,0x20,0x20,0x47,0x90,0x48,0x50,0x48,0xD0,0x47,0x10,0x08,0x20,0x08,0xC0,0x07,	// 64
	0x00,0x08,0x00,0x0E,0xC0,0x03,0x38,0x02,0x38,0x02,0xC0,0x03,0x00,0x0E,0x00,0x08,	// 65
	0x00,0x00,0xF8,0x0F,0x88,0x08,0x88,0x08,0x88,0x08,0x78,0x0D,0x20,0x07,	// 66
	0x00,0x00,0xF0,0x07,0x18,0x0C,0x08,0x08,0x08,0x08,0x08,0x08,0x30,0x06,	// 67
	0x00,0x00,0xF8,0x0F,0x08,0x08,0x08,0x08,0x08,0x08,0x10,0x04,0xE0,0x03,	// 68
	0x00,0x00,0xF8,0x0F,0x88,0x08,0x88,0x08,0x88,0x08,0x88,0x08,	// 69
	0x00,0x00,0xF8,0x0F,0x88,0x00,0x88,0x00,0x88,0x00,0x88,	// 70
	0x00,0x00,0xF0,0x07,0x18,0x0C,0x08,0x08,0x88,0x08,0x88,0x08,0xB0,0x07,	// 71
	0x00,0x00,0xF8,0x0F,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xF8,0x0F,0xF8,0x0F,	// 72
	0x00,0x00,0xF8,0x0F,	// 73
	0x00,0x04,0x00,0x0C,0x00,0x08,0x00,0x08,0x00,0x0C,0xF8,0x03,	// 74
	0x00,0x00,0xF8,0x0F,0x80,0x00,0xC0,0x00,0x20,0x03,0x10,0x06,0x08,0x08,0x00,0x08,	// 75
	0x00,0x00,0xF8,0x0F,0x00,0x08,0x00,0x08,0x00,0x08,0x00,0x08,	// 76
	0x00,0x00,0xF8,0x0F,0x38,0x00,0xC0,0x01,0x00,0x07,0x00,0x0C,0x80,0x03,0x70,0x00,0xF8,0x0F,	// 77
	0x00,0x00,0xF8,0x0F,0x18,0x00,0x60,0x00,0xC0,0x00,0x00,0x03,0x00,0x0C,0xF8,0x0F,	// 78
	0x00,0x00,0xF0,0x07,0x18,0x0C,0x08,0x08,0x08,0x08,0x08,0x08,0xF0,0x07,0xC0,0x01,	// 79
	0x00,0x00,0xF8,0x0F,0x88,0x00,0x88,0x00,0x88,0x00,0x88,0x00,0x70,	// 80
	0x00,0x00,0xF0,0x07,0x18,0x0C,0x08,0x08,0x08,0x08,0x18,0x0C,0xF0,0x17,	// 81
	0x00,0x00,0xF8,0x0F,0x88,0x00,0x88,0x00,0x88,0x01,0xF8,0x06,0x20,0x08,	// 82
	0x00,0x02,0x70,0x06,0xC8,0x08,0x88,0x08,0x88,0x08,0x30,0x0F,0x20,0x02,	// 83
	0x08,0x00,0x08,0x00,0x08,0x00,0xF8,0x0F,0x08,0x00,0x08,0x00,0x08,	// 84
	0x00,0x00,0xF8,0x07,0x00,0x0C,0x00,0x08,0x00,0x08,0x00,0x0C,0xF8,0x07,	// 85
	0x08,0x00,0x38,0x00,0xC0,0x01,0x00,0x0E,0x00,0x0E,0xC0,0x01,0x38,0x00,0x08,	// 86
	0x08,0x00,0xF8,0x00,0x00,0x0F,0x00,0x0F,0xE0,0x00,0x18,0x00,0xE0,0x01,0x00,0x0E,0x80,0x07,0x78,	// 87
	0x08,0x08,0x18,0x0C,0x30,0x07,0xC0,0x01,0x60,0x03,0x30,0x06,0x08,0x08,	// 88
	0x08,0x00,0x38,0x00,0xE0,0x00,0x80,0x0F,0xC0,0x00,0x30,0x00,0x08,	// 89
	0x00,0x00,0x08,0x0C,0x08,0x0B,0x88,0x08,0x68,0x08,0x18,0x08,0x08,0x08,	// 90
	0x00,0x00,0xFC,0x3F,0x04,0x20,	// 91
	0x08,0x00,0x70,0x00,0xC0,0x01,0x00,0x0E,0x00,0x10,	// 92
	0x04,0x20,0xFC,0x3F,	// 93
	0x40,0x00,0x70,0x00,0x08,0x00,0x70,0x00,0x40,	// 94
	0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,	// 95
	0x08,0x00,0x18,0x00,0x10,	// 96
	0x00,0x00,0x40,0x06,0x20,0x09,0x20,0x09,0x20,0x09,0xC0,0x0F,	// 97
	0x00,0x00,0xF8,0x0F,0x20,0x08,0x20,0x08,0x20,0x08,0xC0,0x07,	// 98
	0x80,0x03,0xC0,0x06,0x20,0x08,0x20,0x08,0x60,0x0C,0x40,0x04,	// 99
	0x00,0x00,0xC0,0x07,0x20,0x08,0x20,0x08,0x20,0x08,0xF8,0x0F,	// 100
	0x80,0x03,0xC0,0x05,0x20,0x09,0x20,0x09,0x60,0x0D,0x80,0x09,	// 101
	0x20,0x00,0xF0,0x0F,0x28,0x00,0x28,	// 102
	0x00,0x00,0xC0,0x27,0x20,0x28,0x20,0x28,0x20,0x28,0xE0,0x1F,	// 103
	0x00,0x00,0xF8,0x0F,0x40,0x00,0x20,0x00,0x20,0x00,0xC0,0x0F,	// 104
	0x00,0x00,0xE8,0x0F,	// 105
	0x00,0x20,0xE8,0x3F,	// 106
	0x00,0x00,0xF8,0x0F,0x80,0x01,0xC0,0x03,0x60,0x0C,0x20,0x08,	// 107
	0x00,0x00,0xF8,0x0F,	// 108
	0x00,0x00,0xE0,0x0F,0x20,0x00,0x20,0x00,0x20,0x00,0xC0,0x0F,0x20,0x00,0x20,0x00,0x20,0x00,0xC0,0x0F,	// 109
	0x00,0x00,0xE0,0x0F,0x40,0x00,0x20,0x00,0x20,0x00,0xC0,0x0F,	// 110
	0x00,0x00,0xC0,0x07,0x20,0x08,0x20,0x08,0x20,0x08,0xC0,0x07,	// 111
	0x00,0x00,0xE0,0x3F,0x20,0x08,0x20,0x08,0x20,0x08,0xC0,0x07,	// 112
	0x00,0x00,0xC0,0x07,0x20,0x08,0x20,0x08,0x20,0x08,0xE0,0x3F,	// 113
	0x00,0x00,0xE0,0x0F,0x20,0x00,0x20,	// 114
	0x00,0x04,0xE0,0x0C,0x20,0x09,0x20,0x09,0x60,0x0F,	// 115
	0x20,0x00,0xF8,0x07,0x20,0x08,0x20,0x08,	// 116
	0x00,0x00,0xE0,0x07,0x00,0x08,0x00,0x08,0x00,0x08,0xE0,0x0F,	// 117
	0x20,0x00,0xE0,0x01,0x00,0x0E,0x00,0x0E,0xE0,0x01,0x20,	// 118
	0x20,0x00,0xC0,0x03,0x00,0x0C,0x80,0x03,0x60,0x00,0x80,0x03,0x00,0x0C,0xC0,0x03,0x20,	// 119
	0x20,0x08,0x60,0x0C,0x80,0x03,0x80,0x03,0x60,0x0C,0x20,0x08,	// 120
	0x20,0x00,0xE0,0x20,0x00,0x3F,0x00,0x07,0xE0,0x00,0x20,	// 121
	0x00,0x00,0x20,0x0C,0x20,0x0B,0xA0,0x09,0x60,0x08,0x00,0x08,	// 122
	0x00,0x01,0x80,0x03,0xF0,0x1E,0x08,0x20,	// 123
	0x00,0x00,0xF8,0x3F,	// 124
	0x08,0x20,0xF0,0x1E,0x80,0x03,0x00,0x01,	// 125
	0x00,0x00,0x80,0x01,0x80,0x00,0x80,0x00,0x00,0x01,0x00,0x01,0x80,0x01,	// 126
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 127
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 128
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 129
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 130
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 131
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 132
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 133
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 134
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 135
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 136
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 137
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 138
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 139
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 140
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 141
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 142
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 143
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 144
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 145
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 146
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 147
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 148
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 149
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 150
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 151
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 152
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 153
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 154
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 155
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 156
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 157
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 158
	0xF8,0x0F,0x68,0x0B,0xE8,0x0B,0x28,0x0C,0xF8,0x0F,	// 159
	0x00,0x00,0xA0,0x3F,	// 161
	0x00,0x00,0xC0,0x07,0x20,0x08,0x30,0x18,0x20,0x08,0x40,0x04,	// 162
	0x00,0x00,0xE0,0x0C,0xF8,0x0F,0x88,0x08,0x08,0x08,0x18,0x08,0x00,0x08,	// 163
	0x00,0x00,0xA0,0x0B,0x40,0x04,0x20,0x08,0x20,0x08,0x20,0x08,0x40,0x04,0xA0,0x0B,	// 164
	0x08,0x00,0xB0,0x02,0xC0,0x0F,0xC0,0x0F,0xB0,0x02,0x08,	// 165
	0x00,0x00,0xF8,0x3E,	// 166
	0x00,0x11,0xF0,0x66,0xC8,0x44,0x88,0x44,0x88,0x48,0xB0,0x3F,0x00,0x02,	// 167
	0x00,0x00,0x10,0x00,0x00,0x00,0x10,	// 168
	0xC0,0x01,0x30,0x06,0xD0,0x05,0x28,0x0A,0x28,0x0A,0x68,0x0B,0x10,0x04,0x30,0x06,0xC0,0x01,	// 169
	0x00,0x00,0xF8,0x00,0xA8,0x00,0xF8,	// 170
	0x00,0x00,0x80,0x03,0x40,0x04,0x80,0x03,0x40,0x04,	// 171
	0x00,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x01,	// 172
	0x00,0x01,0x00,0x01,0x00,0x01,	// 173
	0xC0,0x01,0x30,0x06,0x10,0x04,0xE8,0x0B,0xA8,0x08,0xE8,0x0B,0x10,0x04,0x30,0x06,0xC0,0x01,	// 174
	0x00,0x00,0x10,0x00,0x10,0x00,0x10,0x00,0x10,	// 175
	0x00,0x00,0x38,0x00,0x28,0x00,0x10,	// 176
	0x00,0x00,0x80,0x08,0x80,0x08,0xE0,0x0B,0x80,0x08,0x80,0x08,	// 177
	0x90,0x00,0xC8,0x00,0xA8,0x00,0x90,	// 178
	0x90,0x00,0xA8,0x00,0xA8,0x00,0x50,	// 179
	0x00,0x00,0x18,0x00,0x08,	// 180
	0x00,0x00,0xE0,0x3F,0x00,0x08,0x00,0x08,0x00,0x08,0xE0,0x0F,	// 181
	0x20,0x00,0xF0,0x00,0xF8,0x01,0xF8,0x01,0xF8,0x0F,	// 182
	0x00,0x00,0x80,	// 183
	0x00,0x00,0x00,0x30,0x00,0x20,	// 184
	0x00,0x00,0x10,0x00,0xF8,	// 185
	0x20,0x00,0xD8,0x00,0x88,0x00,0xD8,0x00,0x20,	// 186
	0x00,0x00,0xC0,0x06,0x00,0x01,0xC0,0x06,0x00,0x01,	// 187
	0x00,0x00,0x10,0x00,0xF8,0x04,0x00,0x02,0x80,0x05,0x60,0x07,0x90,0x0F,0x00,0x04,	// 188
	0x00,0x00,0x10,0x04,0xF8,0x02,0x80,0x01,0x60,0x00,0x10,0x09,0x80,0x0C,0x80,0x0A,0x00,0x09,	// 189
	0x90,0x00,0x88,0x00,0xA8,0x04,0xD0,0x02,0x80,0x01,0x60,0x04,0x10,0x07,0x80,0x0F,0x00,0x04,	// 190
	0x00,0x00,0x00,0x3C,0x20,0x23,0x20,0x21,0x00,0x30,	// 191
	0x00,0x08,0x00,0x0E,0xC1,0x03,0x3B,0x02,0x3A,0x02,0xC0,0x03,0x00,0x0E,0x00,0x08,	// 192
	0x00,0x08,0x00,0x0E,0xC0,0x03,0x38,0x02,0x3B,0x02,0xC1,0x03,0x00,0x0E,0x00,0x08,	// 193
	0x00,0x08,0x00,0x0E,0xC2,0x03,0x39,0x02,0x3B,0x02,0xC2,0x03,0x00,0x0E,0x00,0x08,	// 194
	0x00,0x08,0x00,0x0E,0xC3,0x03,0x39,0x02,0x3A,0x02,0xC1,0x03,0x00,0x0E,0x00,0x08,	// 195
	0x00,0x08,0x00,0x0E,0xC2,0x03,0x38,0x02,0x3A,0x02,0xC0,0x03,0x00,0x0E,0x00,0x08,	// 196
	0x00,0x08,0x00,0x0E,0xC1,0x03,0x3A,0x02,0x3B,0x02,0xC0,0x03,0x00,0x0E,0x00,0x08,	// 197
	0x00,0x08,0x00,0x0E,0x80,0x03,0xC0,0x02,0x30,0x02,0x08,0x02,0xF8,0x0F,0x88,0x08,0x88,0x08,0x88,0x08,0x08,0x08,	// 198
	0x00,0x00,0xF0,0x07,0x18,0x0C,0x08,0x08,0x08,0x38,0x08,0x28,0x30,0x06,	// 199
	0x00,0x00,0xF9,0x0F,0x8B,0x08,0x8A,0x08,0x88,0x08,0x88,0x08,	// 200
	0x00,0x00,0xF8,0x0F,0x88,0x08,0x8B,0x08,0x89,0x08,0x88,0x08,	// 201
	0x00,0x00,0xF8,0x0F,0x8A,0x08,0x89,0x08,0x8B,0x08,0x8A,0x08,	// 202
	0x00,0x00,0xF8,0x0F,0x8A,0x08,0x88,0x08,0x8A,0x08,0x88,0x08,	// 203
	0x03,0x00,0xFA,0x0F,	// 204
	0x00,0x00,0xFB,0x0F,0x01,	// 205
	0x02,0x00,0xF9,0x0F,0x03,	// 206
	0x02,0x00,0xF8,0x0F,0x02,	// 207
	0x80,0x00,0xF8,0x0F,0x88,0x08,0x88,0x08,0x08,0x08,0x18,0x04,0xF0,0x07,	// 208
	0x00,0x00,0xF8,0x0F,0x1B,0x00,0x61,0x00,0xC2,0x00,0x01,0x03,0x00,0x0C,0xF8,0x0F,	// 209
	0x00,0x00,0xF0,0x07,0x19,0x0C,0x0B,0x08,0x0A,0x08,0x08,0x08,0xF0,0x07,0xC0,0x01,	// 210
	0x00,0x00,0xF0,0x07,0x18,0x0C,0x08,0x08,0x0B,0x08,0x09,0x08,0xF0,0x07,0xC0,0x01,	// 211
	0x00,0x00,0xF0,0x07,0x1A,0x0C,0x09,0x08,0x0B,0x08,0x0A,0x08,0xF0,0x07,0xC0,0x01,	// 212
	0x00,0x00,0xF0,0x07,0x1B,0x0C,0x09,0x08,0x0A,0x08,0x09,0x08,0xF0,0x07,0xC0,0x01,	// 213
	0x00,0x00,0xF0,0x07,0x18,0x0C,0x0A,0x08,0x08,0x08,0x0A,0x08,0xF0,0x07,0xC0,0x01,	// 214
	0x00,0x00,0x60,0x06,0xC0,0x03,0x80,0x01,0x40,0x02,0x20,0x04,	// 215
	0x00,0x00,0xF0,0x0F,0x18,0x0E,0x08,0x09,0xC8,0x08,0x38,0x0C,0xF8,0x07,0x08,	// 216
	0x00,0x00,0xF8,0x07,0x01,0x0C,0x03,0x08,0x02,0x08,0x00,0x0C,0xF8,0x07,	// 217
	0x00,0x00,0xF8,0x07,0x00,0x0C,0x00,0x08,0x03,0x08,0x01,0x0C,0xF8,0x07,	// 218
	0x00,0x00,0xF8,0x07,0x02,0x0C,0x01,0x08,0x03,0x08,0x02,0x0C,0xF8,0x07,	// 219
	0x00,0x00,0xF8,0x07,0x02,0x0C,0x00,0x08,0x02,0x08,0x00,0x0C,0xF8,0x07,	// 220
	0x08,0x00,0x38,0x00,0xE0,0x00,0x83,0x0F,0xC1,0x00,0x30,0x00,0x08,	// 221
	0x00,0x00,0xF8,0x0F,0x20,0x02,0x20,0x02,0x20,0x02,0xC0,0x01,0x80,	// 222
	0x00,0x00,0xF0,0x0F,0x08,0x00,0x88,0x08,0xF8,0x09,0x00,0x0F,0x00,0x04,	// 223
	0x00,0x00,0x44,0x06,0x2C,0x09,0x28,0x09,0x20,0x09,0xC0,0x0F,	// 224
	0x00,0x00,0x40,0x06,0x20,0x09,0x2C,0x09,0x24,0x09,0xC0,0x0F,	// 225
	0x00,0x00,0x40,0x06,0x28,0x09,0x24,0x09,0x2C,0x09,0xC8,0x0F,	// 226
	0x00,0x00,0x40,0x06,0x2C,0x09,0x24,0x09,0x28,0x09,0xC4,0x0F,	// 227
	0x00,0x00,0x40,0x06,0x28,0x09,0x20,0x09,0x28,0x09,0xC0,0x0F,	// 228
	0x00,0x00,0x44,0x06,0x2A,0x09,0x2E,0x09,0x20,0x09,0xC0,0x0F,	// 229
	0x00,0x06,0x60,0x0F,0x20,0x09,0x20,0x09,0xE0,0x07,0xC0,0x05,0x20,0x09,0x20,0x09,0x60,0x0D,0x80,0x09,	// 230
	0x80,0x03,0xC0,0x06,0x20,0x08,0x20,0x38,0x60,0x2C,0x40,0x04,	// 231
	0x80,0x03,0xC4,0x05,0x2C,0x09,0x28,0x09,0x60,0x0D,0x80,0x09,	// 232
	0x80,0x03,0xC0,0x05,0x20,0x09,0x2C,0x09,0x64,0x0D,0x80,0x09,	// 233
	0x80,0x03,0xC0,0x05,0x28,0x09,0x24,0x09,0x6C,0x0D,0x88,0x09,	// 234
	0x80,0x03,0xC0,0x05,0x28,0x09,0x20,0x09,0x68,0x0D,0x80,0x09,	// 235
	0x0C,0x00,0xE8,0x0F,	// 236
	0x00,0x00,0xEC,0x0F,0x04,	// 237
	0x08,0x00,0xE4,0x0F,0x0C,	// 238
	0x08,0x00,0xE0,0x0F,0x08,	// 239
	0x00,0x00,0x90,0x07,0x68,0x08,0x50,0x08,0x78,0x08,0xF0,0x07,	// 240
	0x00,0x00,0xE0,0x0F,0x4C,0x00,0x24,0x00,0x28,0x00,0xC4,0x0F,	// 241
	0x00,0x00,0xC4,0x07,0x2C,0x08,0x28,0x08,0x20,0x08,0xC0,0x07,	// 242
	0x00,0x00,0xC0,0x07,0x20,0x08,0x2C,0x08,0x24,0x08,0xC0,0x07,	// 243
	0x00,0x00,0xC0,0x07,0x28,0x08,0x24,0x08,0x2C,0x08,0xC8,0x07,	// 244
	0x00,0x00,0xC0,0x07,0x2C,0x08,0x24,0x08,0x28,0x08,0xC4,0x07,	// 245
	0x00,0x00,0xC0,0x07,0x28,0x08,0x20,0x08,0x28,0x08,0xC0,0x07,	// 246
	0x00,0x00,0x80,0x00,0x80,0x00,0xA0,0x02,0x80,0x00,0x80,	// 247
	0x00,0x00,0xC0,0x17,0x20,0x0E,0xA0,0x09,0x60,0x08,0xD0,0x07,	// 248
	0x00,0x00,0xE4,0x07,0x0C,0x08,0x08,0x08,0x00,0x08,0xE0,0x0F,	// 249
	0x00,0x00,0xE0,0x07,0x00,0x08,0x0C,0x08,0x04,0x08,0xE0,0x0F,	// 250
	0x00,0x00,0xE0,0x07,0x08,0x08,0x04,0x08,0x0C,0x08,0xE8,0x0F,	// 251
	0x00,0x00,0xE0,0x07,0x08,0x08,0x00,0x08,0x08,0x08,0xE0,0x0F,	// 252
	0x20,0x00,0xE0,0x20,0x00,0x3F,0x0C,0x07,0xE4,0x00,0x20,	// 253
	0x00,0x00,0xF8,0x3F,0x20,0x08,0x20,0x08,0x20,0x08,0xC0,0x07,	// 254
	0x20,0x00,0xE0,0x20,0x08,0x3F,0x00,0x07,0xE8,0x00,0x20	// 255
};
