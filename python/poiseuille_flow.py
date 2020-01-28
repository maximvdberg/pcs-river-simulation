
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit
import numpy as np

viscosity = 0.01
u_slope = (0.1, 0.0)
u0 = ...
delta_x = 1.0
delta_t = 1.0
rho0 = 1.0


u2 = [0.00047074, 0.00140883, 0.00234017, 0.00326489, 0.00418333, 0.00509601, 0.00600349, 0.00690637, 0.00780517, 0.00870033, 0.00959214, 0.0104808, 0.0113664, 0.012249, 0.0131284, 0.0140045, 0.0148772, 0.0157463, 0.0166117, 0.0174731, 0.0183303, 0.0191833, 0.0200317, 0.0208755, 0.0217144, 0.0225483, 0.0233771, 0.0242005, 0.0250185, 0.0258308, 0.0266373, 0.0274378, 0.0282322, 0.0290203, 0.029802, 0.0305772, 0.0313456, 0.0321071, 0.0328617, 0.0336091, 0.0343492, 0.0350819, 0.035807, 0.0365244, 0.0372341, 0.0379358, 0.0386294, 0.0393148, 0.039992, 0.0406607, 0.0413209, 0.0419725, 0.0426154, 0.0432493, 0.0438744, 0.0444904, 0.0450973, 0.0456949, 0.0462832, 0.0468621, 0.0474315, 0.0479913, 0.0485415, 0.0490819, 0.0496126, 0.0501333, 0.0506441, 0.0511449, 0.0516356, 0.0521162, 0.0525865, 0.0530466, 0.0534964, 0.0539357, 0.0543647, 0.0547832, 0.0551912, 0.0555886, 0.0559754, 0.0563515, 0.056717, 0.0570717, 0.0574156, 0.0577488, 0.0580711, 0.0583826, 0.0586831, 0.0589728, 0.0592515, 0.0595192, 0.0597759, 0.0600216, 0.0602563, 0.0604799, 0.0606924, 0.0608939, 0.0610842, 0.0612634, 0.0614315, 0.0615884, 0.0617342, 0.0618687, 0.0619921, 0.0621043, 0.0622053, 0.0622951, 0.0623737, 0.0624411, 0.0624972, 0.0625421, 0.0625758, 0.0625983, 0.0626095, 0.0626095, 0.0625982, 0.0625757, 0.062542, 0.062497, 0.0624408, 0.0623734, 0.0622948, 0.062205, 0.0621039, 0.0619917, 0.0618682, 0.0617336, 0.0615878, 0.0614309, 0.0612627, 0.0610835, 0.0608931, 0.0606916, 0.0604791, 0.0602554, 0.0600207, 0.059775, 0.0595182, 0.0592504, 0.0589717, 0.058682, 0.0583814, 0.05807, 0.0577476, 0.0574144, 0.0570704, 0.0567157, 0.0563502, 0.055974, 0.0555872, 0.0551898, 0.0547818, 0.0543633, 0.0539343, 0.0534949, 0.0530451, 0.052585, 0.0521146, 0.0516341, 0.0511433, 0.0506426, 0.0501317, 0.049611, 0.0490803, 0.0485399, 0.0479897, 0.0474299, 0.0468605, 0.0462815, 0.0456932, 0.0450956, 0.0444887, 0.0438727, 0.0432477, 0.0426137, 0.0419709, 0.0413193, 0.0406591, 0.0399904, 0.0393132, 0.0386278, 0.0379342, 0.0372325, 0.0365229, 0.0358055, 0.0350803, 0.0343477, 0.0336076, 0.0328602, 0.0321057, 0.0313442, 0.0305758, 0.0298007, 0.029019, 0.0282309, 0.0274365, 0.026636, 0.0258295, 0.0250173, 0.0241994, 0.023376, 0.0225472, 0.0217133, 0.0208744, 0.0200307, 0.0191823, 0.0183294, 0.0174722, 0.0166108, 0.0157455, 0.0148764, 0.0140038, 0.0131277, 0.0122483, 0.0113658, 0.0104803, 0.00959164, 0.00869987, 0.00780477, 0.00690601, 0.00600318, 0.00509574, 0.00418311, 0.00326472, 0.00234004, 0.00140876, 0.000470715]

#u2 = [0.0181175, 0.0373399, 0.0551884, 0.0716536, 0.0866563, 0.100117, 0.111978, 0.122254, 0.131013, 0.138366, 0.144446, 0.149393, 0.153345, 0.15644, 0.158804, 0.160555, 0.161798, 0.162624, 0.163111, 0.163325, 0.163321, 0.163144, 0.162831, 0.16241, 0.161906, 0.161335, 0.160711, 0.160044, 0.159339, 0.158599, 0.157825, 0.157013, 0.156158, 0.155248, 0.154273, 0.153213, 0.152047, 0.150747, 0.14928, 0.147607, 0.145682, 0.143453, 0.140861, 0.13784, 0.134323, 0.130234, 0.125499, 0.120045, 0.1138, 0.106706, 0.0987133, 0.0897839, 0.0798805, 0.0689593, 0.0569629, 0.0438284, 0.0294925, 0.0139148, ]

# Omega  v= 0.005
#u2 = [0.0258561, 0.0568558, 0.0885507, 0.115825, 0.135519, 0.148617, 0.156215, 0.160334, 0.16218, 0.162761, 0.16262, 0.162126, 0.16137, 0.160629, 0.15982, 0.158954, 0.158175, 0.157315, 0.156517, 0.155728, 0.154945, 0.154135, 0.153385, 0.152633, 0.15186, 0.151135, 0.150415, 0.149675, 0.148975, 0.148277, 0.147577, 0.146905, 0.146226, 0.145565, 0.144907, 0.144257, 0.14362, 0.143003, 0.142364, 0.141765, 0.141147, 0.14056, 0.13995, 0.139357, 0.138754, 0.13814, 0.137413, 0.136489, 0.135175, 0.133201, 0.129975, 0.124854, 0.116853, 0.105044, 0.0886625, 0.0679107, 0.0436605, 0.0175948, ]

# Omega v = 0.001
#u2 = [0.0316473, 0.0733748, 0.117727, 0.135354, 0.142988, 0.152413, 0.14792, 0.14595, 0.150591, 0.146546, 0.150596, 0.154977, 0.148345, 0.145303, 0.147656, 0.152024, 0.149155, 0.144575, 0.148324, 0.14806, 0.14618, 0.143345, 0.145515, 0.146128, 0.142441, 0.141709, 0.143457, 0.141813, 0.139911, 0.140611, 0.140272, 0.138335, 0.137672, 0.138813, 0.137776, 0.134621, 0.135641, 0.136725, 0.135187, 0.132923, 0.133039, 0.134142, 0.132313, 0.131351, 0.130897, 0.131632, 0.130349, 0.129532, 0.128452, 0.128924, 0.128548, 0.127576, 0.126632, 0.125738, 0.119805, 0.0970397, 0.0604517, 0.0201464, ]

#omega 0.01
#u2 = [0.0191841, 0.0414685, 0.0643171, 0.0862629, 0.105733, 0.121772, 0.134164, 0.143269, 0.149559, 0.153708, 0.156227, 0.157624, 0.158232, 0.158291, 0.158038, 0.157552, 0.156936, 0.156247, 0.155516, 0.154754, 0.153997, 0.153235, 0.152473, 0.151723, 0.150983, 0.150246, 0.149521, 0.148809, 0.148102, 0.147404, 0.146718, 0.14604, 0.145369, 0.144708, 0.144054, 0.143409, 0.142768, 0.142132, 0.141492, 0.140848, 0.140176, 0.139466, 0.13867, 0.137742, 0.136595, 0.135117, 0.133132, 0.130435, 0.12674, 0.121738, 0.115069, 0.106403, 0.0954958, 0.0822987, 0.0669721, 0.0498766, 0.0315558, 0.0128972, ]

#lambda binnen
#u2 =[0.0177684, 0.0673653, 0.122741, 0.162876, 0.178498, 0.183396, 0.184894, 0.185319, 0.183434, 0.180929, 0.178731, 0.176325, 0.173355, 0.170473, 0.16787, 0.165187, 0.162417, 0.159824, 0.157383, 0.154946, 0.15257, 0.15033, 0.148171, 0.146057, 0.144028, 0.14209, 0.140206, 0.138383, 0.136637, 0.134952, 0.133316, 0.131741, 0.130225, 0.128756, 0.127332, 0.125958, 0.124627, 0.123328, 0.122062, 0.120826, 0.119607, 0.118396, 0.117187, 0.115965, 0.114712, 0.11341, 0.112039, 0.110571, 0.108977, 0.107225, 0.105285, 0.103123, 0.100712, 0.0980291, 0.095062, 0.0918079, 0.0882755, 0.0844845, 0.080463, 0.0762434, 0.0718581, 0.0673392, 0.0627156, 0.0580121, 0.0532503, 0.0484542, 0.0436486, 0.0388604, 0.0341222, 0.029481, 0.0249952, 0.0207486, 0.0168777, 0.0136218, 0.0113625, 0.010559, 0.0113242, 0.0132013, 0.0156462, 0.0183012, 0.0209856, 0.0235917, 0.0261024, 0.028464, 0.0306549, 0.0326005, 0.0341917, 0.035165, 0.0351603, 0.0335888, 0.0296679, 0.0224583, 0.0110226, ]

# river3 v = 0.01, u0=0.1, x only
#u2 = [0.00888361, 0.0218282, 0.0341482, 0.0457931, 0.0564907, 0.0658157, 0.0735309, 0.0796424, 0.0843888, 0.0880839, 0.0910088, 0.0933811, 0.0953637, 0.097045, 0.098504, 0.0997861, 0.100936, 0.101983, 0.102958, 0.103878, 0.104764, 0.105621, 0.106457, 0.107271, 0.108059, 0.108803, 0.10949, 0.110088, 0.110559, 0.110849, 0.110899, 0.110607, 0.109853, 0.108477, 0.106242, 0.102843, 0.097947, 0.0912147, 0.0823632, 0.0713668, 0.0585539, 0.0446135, 0.0307544, 0.0182134, 0.00785, ]

# river 3 v = 0.01, u0=.1, x only
#u2 = [0.0698369, 0.139468, 0.193497, 0.231282, 0.253677, 0.266931, 0.272778, 0.275075, 0.275586, 0.274155, 0.27288, 0.270586, 0.268626, 0.266437, 0.264221, 0.262102, 0.259849, 0.257696, 0.255618, 0.253469, 0.25148, 0.249504, 0.247506, 0.245619, 0.243762, 0.241869, 0.24009, 0.238326, 0.236563, 0.234796, 0.233093, 0.23125, 0.229596, 0.22759, 0.225644, 0.223448, 0.220913, 0.217497, 0.212608, 0.204944, 0.192819, 0.173469, 0.144538, 0.103037, 0.048288, ]

# river 2, v=0.001, u0=0.05, x only
#u2 = [0.026024, 0.053465, 0.0721717, 0.0780624, 0.079152, 0.0783645, 0.0779268, 0.0775129, 0.0763635, 0.0761432, 0.0749451, 0.0745797, 0.0737996, 0.0731391, 0.0728064, 0.0719292, 0.071662, 0.0707508, 0.0702584, 0.06982, 0.0689682, 0.0687378, 0.0681424, 0.0674682, 0.0672322, 0.0664847, 0.0660726, 0.0656749, 0.0651927, 0.064466, 0.0644092, 0.0637706, 0.0634636, 0.0630397, 0.0624883, 0.0621236, 0.0617372, 0.0614228, 0.0609414, 0.0606928, 0.0600028, 0.059053, 0.0532095, 0.0394369, 0.0181834, ]

u2 = [0.00092007, 0.00192375, 0.00291762, 0.00390196, 0.00487735, 0.00584457, 0.00680441, 0.00775755, 0.00870453, 0.00964561, 0.0105809, 0.0115102, 0.0124332, 0.0133496, 0.0142589, 0.0151606, 0.0160541, 0.0169389, 0.0178146, 0.0186806, 0.0195366, 0.0203821, 0.0212167, 0.0220401, 0.0228519, 0.0236518, 0.0244395, 0.0252148, 0.0259773, 0.0267269, 0.0274633, 0.0281864, 0.0288959, 0.0295917, 0.0302737, 0.0309417, 0.0315956, 0.0322353, 0.0328608, 0.033472, 0.0340689, 0.0346514, 0.0352196, 0.0357735, 0.036313, 0.0368384, 0.0373495, 0.0378465, 0.0383295, 0.0387986, 0.0392539, 0.0396955, 0.0401236, 0.0405383, 0.0409397, 0.0413281, 0.0417037, 0.0420665, 0.0424168, 0.0427548, 0.0430807, 0.0433947, 0.043697, 0.0439878, 0.0442674, 0.0445359, 0.0447937, 0.0450409, 0.0452777, 0.0455044, 0.0457211, 0.0459282, 0.0461259, 0.0463142, 0.0464936, 0.0466641, 0.046826, 0.0469796, 0.0471249, 0.0472621, 0.0473916, 0.0475134, 0.0476277, 0.0477347, 0.0478346, 0.0479274, 0.0480134, 0.0480927, 0.0481654, 0.0482316, 0.0482914, 0.048345, 0.0483924, 0.0484337, 0.0484689, 0.0484982, 0.0485216, 0.0485391, 0.0485508, 0.0485566, 0.0485567, 0.0485509, 0.0485393, 0.0485218, 0.0484985, 0.0484693, 0.0484341, 0.0483929, 0.0483455, 0.048292, 0.0482323, 0.0481662, 0.0480936, 0.0480144, 0.0479285, 0.0478357, 0.0477359, 0.047629, 0.0475148, 0.0473931, 0.0472638, 0.0471266, 0.0469814, 0.046828, 0.0466663, 0.0464959, 0.0463167, 0.0461284, 0.045931, 0.045724, 0.0455075, 0.045281, 0.0450443, 0.0447974, 0.0445399, 0.0442715, 0.0439922, 0.0437016, 0.0433996, 0.0430859, 0.0427603, 0.0424226, 0.0420726, 0.0417101, 0.041335, 0.040947, 0.0405459, 0.0401316, 0.039704, 0.0392628, 0.038808, 0.0383394, 0.0378569, 0.0373605, 0.0368499, 0.0363252, 0.0357862, 0.035233, 0.0346655, 0.0340836, 0.0334875, 0.032877, 0.0322523, 0.0316134, 0.0309603, 0.0302932, 0.0296121, 0.0289172, 0.0282086, 0.0274866, 0.0267511, 0.0260026, 0.0252411, 0.024467, 0.0236804, 0.0228817, 0.0220711, 0.021249, 0.0204157, 0.0195716, 0.018717, 0.0178525, 0.0169784, 0.0160953, 0.0152037, 0.0143042, 0.0133972, 0.0124834, 0.0115632, 0.0106368, 0.00970459, 0.00876633, 0.00782166, 0.00686985, 0.00590984, 0.0049403, 0.00395977, 0.00296682, 0.00196032, 0.0009396, ]

u2 = [0.000718127, 0.00149522, 0.00225963, 0.00301223, 0.00375437, 0.00448766, 0.00521377, 0.00593422, 0.00665028, 0.00736288, 0.00807262, 0.0087798, 0.00948447, 0.0101865, 0.0108857, 0.0115818, 0.0122744, 0.0129632, 0.0136479, 0.0143282, 0.015004, 0.0156748, 0.0163406, 0.017001, 0.0176559, 0.0183052, 0.0189485, 0.0195858, 0.0202168, 0.0208414, 0.0214595, 0.0220708, 0.0226752, 0.0232726, 0.0238628, 0.0244456, 0.025021, 0.0255888, 0.0261489, 0.0267011, 0.0272453, 0.0277815, 0.0283095, 0.0288292, 0.0293406, 0.0298435, 0.0303378, 0.0308235, 0.0313005, 0.0317687, 0.0322281, 0.0326786, 0.0331202, 0.0335527, 0.0339762, 0.0343906, 0.0347958, 0.0351919, 0.0355788, 0.0359565, 0.0363249, 0.036684, 0.0370338, 0.0373744, 0.0377056, 0.0380275, 0.0383401, 0.0386433, 0.0389372, 0.0392217, 0.039497, 0.0397629, 0.0400194, 0.0402667, 0.0405046, 0.0407333, 0.0409527, 0.0411628, 0.0413636, 0.0415552, 0.0417375, 0.0419106, 0.0420745, 0.0422293, 0.0423748, 0.0425112, 0.0426384, 0.0427565, 0.0428655, 0.0429653, 0.0430561, 0.0431377, 0.0432103, 0.0432738, 0.0433282, 0.0433736, 0.0434099, 0.0434372, 0.0434554, 0.0434646, 0.0434647, 0.0434558, 0.0434379, 0.0434109, 0.0433749, 0.0433298, 0.0432757, 0.0432125, 0.0431402, 0.0430589, 0.0429685, 0.0428689, 0.0427603, 0.0426425, 0.0425156, 0.0423796, 0.0422344, 0.04208, 0.0419165, 0.0417437, 0.0415618, 0.0413706, 0.0411702, 0.0409605, 0.0407415, 0.0405133, 0.0402758, 0.040029, 0.0397729, 0.0395075, 0.0392327, 0.0389487, 0.0386553, 0.0383526, 0.0380406, 0.0377193, 0.0373886, 0.0370487, 0.0366995, 0.036341, 0.0359732, 0.0355962, 0.03521, 0.0348147, 0.0344101, 0.0339965, 0.0335738, 0.033142, 0.0327013, 0.0322517, 0.0317931, 0.0313258, 0.0308497, 0.0303649, 0.0298716, 0.0293697, 0.0288594, 0.0283407, 0.0278138, 0.0272787, 0.0267356, 0.0261846, 0.0256258, 0.0250592, 0.0244851, 0.0239035, 0.0233147, 0.0227187, 0.0221157, 0.0215058, 0.0208892, 0.0202661, 0.0196365, 0.0190008, 0.018359, 0.0177114, 0.0170581, 0.0163993, 0.0157352, 0.015066, 0.014392, 0.0137134, 0.0130306, 0.0123438, 0.0116533, 0.0109597, 0.0102631, 0.00956401, 0.00886254, 0.00815875, 0.0074524, 0.00674289, 0.00602918, 0.00530971, 0.00458247, 0.00384507, 0.00309494, 0.00232962, 0.00154705, 0.000745858, ]
u2 = [0.000601353, 0.00124518, 0.00187284, 0.0024856, 0.00308533, 0.00367427, 0.00425477, 0.00482901, 0.00539882, 0.00596562, 0.00653036, 0.00709359, 0.00765554, 0.0082162, 0.00877541, 0.00933294, 0.00988851, 0.0104419, 0.0109927, 0.0115409, 0.0120862, 0.0126283, 0.0131673, 0.0137028, 0.0142348, 0.0147631, 0.0152877, 0.0158083, 0.0163248, 0.0168372, 0.0173452, 0.0178488, 0.0183479, 0.0188423, 0.0193318, 0.0198165, 0.0202961, 0.0207705, 0.0212397, 0.0217034, 0.0221617, 0.0226143, 0.0230613, 0.0235024, 0.0239376, 0.0243667, 0.0247898, 0.0252066, 0.0256171, 0.0260211, 0.0264187, 0.0268097, 0.027194, 0.0275716, 0.0279423, 0.0283061, 0.0286628, 0.0290125, 0.0293551, 0.0296904, 0.0300185, 0.0303392, 0.0306524, 0.0309582, 0.0312564, 0.031547, 0.0318299, 0.0321051, 0.0323725, 0.0326321, 0.0328838, 0.0331276, 0.0333634, 0.0335911, 0.0338108, 0.0340224, 0.0342258, 0.0344211, 0.0346081, 0.0347869, 0.0349573, 0.0351195, 0.0352733, 0.0354187, 0.0355558, 0.0356843, 0.0358045, 0.0359162, 0.0360194, 0.036114, 0.0362002, 0.0362778, 0.0363468, 0.0364073, 0.0364592, 0.0365025, 0.0365373, 0.0365634, 0.0365809, 0.0365899, 0.0365902, 0.0365819, 0.036565, 0.0365395, 0.0365054, 0.0364628, 0.0364115, 0.0363517, 0.0362833, 0.0362063, 0.0361208, 0.0360268, 0.0359243, 0.0358133, 0.0356939, 0.035566, 0.0354296, 0.0352849, 0.0351318, 0.0349704, 0.0348007, 0.0346226, 0.0344364, 0.0342419, 0.0340392, 0.0338284, 0.0336095, 0.0333825, 0.0331476, 0.0329046, 0.0326538, 0.0323951, 0.0321285, 0.0318542, 0.0315722, 0.0312825, 0.0309852, 0.0306804, 0.0303681, 0.0300484, 0.0297214, 0.029387, 0.0290455, 0.0286969, 0.0283411, 0.0279784, 0.0276088, 0.0272324, 0.0268493, 0.0264595, 0.0260631, 0.0256602, 0.025251, 0.0248355, 0.0244137, 0.0239859, 0.023552, 0.0231123, 0.0226668, 0.0222155, 0.0217587, 0.0212965, 0.0208288, 0.0203559, 0.0198779, 0.0193948, 0.0189069, 0.0184142, 0.0179168, 0.0174149, 0.0169085, 0.0163979, 0.0158832, 0.0153644, 0.0148416, 0.0143151, 0.013785, 0.0132513, 0.0127142, 0.012174, 0.0116306, 0.0110844, 0.0105356, 0.00998448, 0.00943129, 0.00887639, 0.00832011, 0.00776271, 0.00720433, 0.00664487, 0.00608387, 0.00552041, 0.00495294, 0.00437929, 0.00379667, 0.00320184, 0.00259136, 0.00196197, 0.00131097, 0.000636547, ]
u2 = [0.00057274, 0.00118023, 0.00176739, 0.00233633, 0.00289005, 0.00343199, 0.00396547, 0.0044933, 0.00501759, 0.00553972, 0.00606046, 0.00658012, 0.00709874, 0.00761619, 0.00813224, 0.00864666, 0.00915922, 0.00966971, 0.0101779, 0.0106837, 0.0111869, 0.0116873, 0.0121849, 0.0126794, 0.0131708, 0.013659, 0.0141438, 0.014625, 0.0151027, 0.0155766, 0.0160467, 0.0165128, 0.0169749, 0.0174327, 0.0178862, 0.0183353, 0.0187799, 0.0192198, 0.019655, 0.0200853, 0.0205107, 0.020931, 0.0213461, 0.021756, 0.0221605, 0.0225595, 0.022953, 0.0233408, 0.0237229, 0.0240992, 0.0244695, 0.0248339, 0.0251921, 0.0255442, 0.02589, 0.0262295, 0.0265625, 0.0268891, 0.0272091, 0.0275225, 0.0278291, 0.028129, 0.0284221, 0.0287082, 0.0289874, 0.0292595, 0.0295245, 0.0297824, 0.030033, 0.0302764, 0.0305125, 0.0307412, 0.0309624, 0.0311763, 0.0313825, 0.0315813, 0.0317724, 0.0319559, 0.0321317, 0.0322998, 0.0324601, 0.0326126, 0.0327574, 0.0328942, 0.0330232, 0.0331443, 0.0332574, 0.0333626, 0.0334598, 0.033549, 0.0336302, 0.0337034, 0.0337685, 0.0338256, 0.0338745, 0.0339154, 0.0339482, 0.0339729, 0.0339895, 0.033998, 0.0339984, 0.0339907, 0.0339749, 0.033951, 0.0339189, 0.0338788, 0.0338306, 0.0337744, 0.0337101, 0.0336377, 0.0335573, 0.0334689, 0.0333725, 0.0332681, 0.0331558, 0.0330356, 0.0329074, 0.0327714, 0.0326275, 0.0324758, 0.0323164, 0.0321492, 0.0319742, 0.0317917, 0.0316014, 0.0314036, 0.0311982, 0.0309854, 0.0307651, 0.0305373, 0.0303022, 0.0300598, 0.0298102, 0.0295533, 0.0292893, 0.0290182, 0.0287401, 0.0284551, 0.0281631, 0.0278644, 0.0275588, 0.0272466, 0.0269277, 0.0266024, 0.0262705, 0.0259322, 0.0255877, 0.0252368, 0.0248799, 0.0245168, 0.0241478, 0.0237729, 0.0233922, 0.0230057, 0.0226137, 0.022216, 0.021813, 0.0214046, 0.020991, 0.0205722, 0.0201484, 0.0197197, 0.0192861, 0.0188478, 0.0184049, 0.0179575, 0.0175057, 0.0170496, 0.0165893, 0.016125, 0.0156567, 0.0151846, 0.0147089, 0.0142295, 0.0137466, 0.0132604, 0.0127709, 0.0122783, 0.0117828, 0.0112843, 0.0107832, 0.0102795, 0.00977339, 0.00926511, 0.00875486, 0.00824291, 0.00772953, 0.00721501, 0.00669959, 0.00618339, 0.00566627, 0.00514763, 0.00462617, 0.00409972, 0.00356503, 0.00301791, 0.00245354, 0.00186708, 0.00125443, 0.000612766, ]
u2 = [0.00054768, 0.00109676, 0.0015953, 0.00206251, 0.00251189, 0.00295343, 0.00339292, 0.00383266, 0.00427284, 0.00471282, 0.00515186, 0.00558939, 0.00602512, 0.00645889, 0.0068907, 0.00732055, 0.00774845, 0.0081744, 0.00859839, 0.00902035, 0.00944021, 0.0098579, 0.0102733, 0.0106864, 0.0110969, 0.0115049, 0.0119101, 0.0123126, 0.0127122, 0.0131087, 0.0135021, 0.0138923, 0.0142792, 0.0146627, 0.0150427, 0.0154191, 0.0157918, 0.0161607, 0.0165257, 0.0168868, 0.0172438, 0.0175967, 0.0179454, 0.0182897, 0.0186297, 0.0189652, 0.0192961, 0.0196224, 0.0199439, 0.0202607, 0.0205726, 0.0208795, 0.0211814, 0.0214782, 0.0217698, 0.0220562, 0.0223372, 0.0226129, 0.0228831, 0.0231478, 0.023407, 0.0236604, 0.0239082, 0.0241502, 0.0243864, 0.0246167, 0.0248411, 0.0250595, 0.0252718, 0.025478, 0.0256781, 0.025872, 0.0260597, 0.0262411, 0.0264161, 0.0265848, 0.0267471, 0.0269029, 0.0270523, 0.0271951, 0.0273314, 0.0274611, 0.0275841, 0.0277006, 0.0278103, 0.0279134, 0.0280097, 0.0280993, 0.0281821, 0.0282581, 0.0283273, 0.0283897, 0.0284453, 0.028494, 0.0285358, 0.0285708, 0.0285989, 0.0286201, 0.0286345, 0.0286419, 0.0286424, 0.0286361, 0.0286228, 0.0286027, 0.0285757, 0.0285418, 0.028501, 0.0284534, 0.0283989, 0.0283376, 0.0282695, 0.0281946, 0.0281129, 0.0280244, 0.0279292, 0.0278273, 0.0277187, 0.0276034, 0.0274815, 0.0273529, 0.0272178, 0.0270762, 0.026928, 0.0267733, 0.0266122, 0.0264448, 0.0262709, 0.0260908, 0.0259043, 0.0257117, 0.0255128, 0.0253079, 0.0250968, 0.0248797, 0.0246567, 0.0244277, 0.0241928, 0.0239522, 0.0237058, 0.0234537, 0.0231959, 0.0229326, 0.0226639, 0.0223896, 0.02211, 0.0218251, 0.021535, 0.0212397, 0.0209393, 0.020634, 0.0203236, 0.0200085, 0.0196885, 0.0193639, 0.0190346, 0.0187008, 0.0183625, 0.0180199, 0.0176729, 0.0173218, 0.0169666, 0.0166073, 0.0162441, 0.0158771, 0.0155063, 0.0151318, 0.0147538, 0.0143724, 0.0139875, 0.0135994, 0.0132081, 0.0128138, 0.0124164, 0.0120163, 0.0116133, 0.0112077, 0.0107995, 0.0103889, 0.00997593, 0.00956072, 0.00914334, 0.00872387, 0.00830236, 0.00787882, 0.00745323, 0.00702555, 0.00659572, 0.00616366, 0.00572939, 0.00529309, 0.00485522, 0.00441661, 0.0039784, 0.00354143, 0.00310502, 0.00266493, 0.00221154, 0.00172983, 0.00120184, 0.0006062, ]
u2 = [0.00047778, 0.00097651, 0.00145185, 0.00190569, 0.00234085, 0.00276077, 0.00316905, 0.00356909, 0.00396374, 0.00435519, 0.00474494, 0.00513384, 0.00552231, 0.0059104, 0.00629797, 0.0066848, 0.00707061, 0.00745515, 0.00783819, 0.00821955, 0.00859905, 0.00897659, 0.00935203, 0.0097253, 0.0100963, 0.0104649, 0.0108311, 0.0111947, 0.0115558, 0.0119141, 0.0122696, 0.0126223, 0.012972, 0.0133186, 0.0136621, 0.0140024, 0.0143394, 0.014673, 0.0150031, 0.0153297, 0.0156526, 0.0159718, 0.0162872, 0.0165987, 0.0169063, 0.0172098, 0.0175092, 0.0178045, 0.0180955, 0.0183822, 0.0186645, 0.0189423, 0.0192156, 0.0194843, 0.0197483, 0.0200076, 0.0202621, 0.0205117, 0.0207564, 0.0209961, 0.0212308, 0.0214604, 0.0216849, 0.0219041, 0.0221181, 0.0223268, 0.0225301, 0.022728, 0.0229204, 0.0231073, 0.0232887, 0.0234645, 0.0236346, 0.023799, 0.0239577, 0.0241107, 0.0242579, 0.0243992, 0.0245346, 0.0246642, 0.0247878, 0.0249054, 0.0250171, 0.0251227, 0.0252223, 0.0253158, 0.0254033, 0.0254846, 0.0255598, 0.0256288, 0.0256917, 0.0257484, 0.0257989, 0.0258431, 0.0258812, 0.025913, 0.0259386, 0.025958, 0.0259711, 0.025978, 0.0259786, 0.025973, 0.0259611, 0.025943, 0.0259186, 0.025888, 0.0258512, 0.0258082, 0.0257589, 0.0257035, 0.0256419, 0.0255741, 0.0255002, 0.0254202, 0.025334, 0.0252418, 0.0251435, 0.0250391, 0.0249288, 0.0248124, 0.0246901, 0.0245619, 0.0244278, 0.0242878, 0.024142, 0.0239904, 0.0238331, 0.02367, 0.0235013, 0.0233269, 0.023147, 0.0229615, 0.0227705, 0.022574, 0.0223722, 0.022165, 0.0219525, 0.0217347, 0.0215118, 0.0212837, 0.0210506, 0.0208124, 0.0205693, 0.0203212, 0.0200683, 0.0198107, 0.0195483, 0.0192813, 0.0190097, 0.0187336, 0.018453, 0.0181681, 0.0178788, 0.0175854, 0.0172877, 0.016986, 0.0166803, 0.0163707, 0.0160572, 0.0157399, 0.0154189, 0.0150943, 0.0147662, 0.0144346, 0.0140997, 0.0137615, 0.0134201, 0.0130755, 0.012728, 0.0123775, 0.0120241, 0.011668, 0.0113092, 0.0109478, 0.0105839, 0.0102176, 0.00984885, 0.00947788, 0.00910473, 0.0087295, 0.00835233, 0.00797335, 0.00759277, 0.00721081, 0.00682778, 0.00644399, 0.00605979, 0.00567545, 0.00529108, 0.0049065, 0.00452102, 0.00413324, 0.0037409, 0.00334076, 0.00292869, 0.0024999, 0.00204935, 0.00157236, 0.00106512, 0.000525107, ]


#other
# u2 = [0.00243002, 0.00509215, 0.00773987, 0.0103717, 0.0129863, 0.0155826, 0.0181592, 0.0207146, 0.0232471, 0.0257547, 0.0282353, 0.0306867, 0.0331069, 0.0354935, 0.0378445, 0.0401579, 0.0424316, 0.0446639, 0.0468531, 0.0489977, 0.0510961, 0.0531473, 0.0551499, 0.0571031, 0.0590059, 0.0608576, 0.0626577, 0.0644056, 0.066101, 0.0677438, 0.0693337, 0.0708709, 0.0723554, 0.0737875, 0.0751676, 0.076496, 0.0777733, 0.0790002, 0.0801773, 0.0813054, 0.0823854, 0.0834183, 0.0844049, 0.0853463, 0.0862436, 0.087098, 0.0879106, 0.0886826, 0.0894153, 0.0901098, 0.0907674, 0.0913895, 0.0919773, 0.0925321, 0.0930551, 0.0935476, 0.094011, 0.0944464, 0.0948552, 0.0952384, 0.0955973, 0.0959331, 0.096247, 0.0965399, 0.0968131, 0.0970675, 0.0973043, 0.0975243, 0.0977285, 0.0979179, 0.0980933, 0.0982556, 0.0984055, 0.0985439, 0.0986714, 0.0987888, 0.0988968, 0.0989959, 0.0990868, 0.09917, 0.0992461, 0.0993155, 0.0993787, 0.0994362, 0.0994883, 0.0995355, 0.099578, 0.0996161, 0.0996503, 0.0996807, 0.0997076, 0.0997312, 0.0997518, 0.0997693, 0.0997841, 0.0997963, 0.0998059, 0.099813, 0.0998177, 0.09982, 0.0998201, 0.0998177, 0.099813, 0.0998059, 0.0997963, 0.0997842, 0.0997694, 0.0997518, 0.0997313, 0.0997077, 0.0996808, 0.0996504, 0.0996163, 0.0995781, 0.0995356, 0.0994885, 0.0994364, 0.099379, 0.0993158, 0.0992464, 0.0991704, 0.0990872, 0.0989963, 0.0988972, 0.0987893, 0.0986719, 0.0985444, 0.0984061, 0.0982563, 0.0980941, 0.0979187, 0.0977294, 0.0975253, 0.0973054, 0.0970687, 0.0968144, 0.0965413, 0.0962485, 0.0959348, 0.0955991, 0.0952404, 0.0948573, 0.0944488, 0.0940135, 0.0935504, 0.093058, 0.0925353, 0.0919808, 0.0913933, 0.0907715, 0.0901141, 0.08942, 0.0886877, 0.0879161, 0.0871039, 0.08625, 0.0853531, 0.0844122, 0.0834261, 0.0823938, 0.0813144, 0.0801869, 0.0790105, 0.0777844, 0.0765078, 0.0751802, 0.073801, 0.0723698, 0.0708862, 0.0693501, 0.0677612, 0.0661195, 0.0644252, 0.0626785, 0.0608797, 0.0590293, 0.0571278, 0.055176, 0.0531748, 0.0511252, 0.0490283, 0.0468854, 0.0446979, 0.0424674, 0.0401957, 0.0378844, 0.0355357, 0.0331516, 0.0307342, 0.0282856, 0.025808, 0.0233034, 0.0207736, 0.0182202, 0.0156443, 0.0130469, 0.0104285, 0.00778935, 0.00512989, 0.00245089, ]

print(len(u2))
x = np.linspace(0.0, 1.0, len(u2))

f = lambda x, h, p_grad : p_grad * (h - x) * x / viscosity

fig = plt.figure(figsize=(4.0,3.8))

popt, pcov = curve_fit(f, x, u2)
print(popt, pcov)

#plt.rcParams['axes.facecolor'] = (0.98431, 0.952941, 0.937254)

plt.plot(u2, x, label="LBM")
plt.plot(f(x, *popt), x, linestyle='--', label="Theortical")
plt.grid()
plt.legend()
plt.ylabel("y position (m)")
plt.xlabel("Horizontal flow velocity (m s$^{-1}$)")
plt.xlim(0, max(u2)*1.1)
plt.ylim(x[0], x[-1])

plt.tight_layout()

plt.savefig('poiseuille.png', transparent=True)
plt.show()


