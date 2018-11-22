from pyflann import *
from numpy import *
from numpy.random import *


data_file_a = open("new_100000_liberty_sift_lda_64_binary.txt")
data_file_b = open("new_100000_notredame_sift_lda_64_binary.txt")
query_file_a = open("new_100000_liberty_sift_lda_64_binary_query.txt")
query_file_b = open("new_100000_notredame_sift_lda_64_binary_query.txt")

data = rand(200000, 64)
query = rand(1000, 64)

id_count = 0
for lin in data_file_a.readlines():
	for i in range(64):
		data[id_count][i] = int(lin[i])
	id_count = id_count + 1
for lin in data_file_b.readlines():
	for i in range(64):
		data[id_count][i] = int(lin[i])
	id_count = id_count + 1

id_count = 0
for lin in query_file_a.readlines():
	if id_count < 500:
		for i in range(64):
			query[id_count][i] = int(lin[i])
	id_count = id_count + 1
id_count = 0
for lin in query_file_b.readlines():
	if id_count < 500:
		for i in range(64):
			query[id_count + 500][i] = int(lin[i])
	id_count = id_count + 1

flann = FLANN()

set_distance_type(2)
params = flann.build_index(data, algorithm = 5, checks = 40000, centers_init = "random", trees = 4, leaf_max_size = 1600, branching = 16);

print(params)

result, dist = flann.nn_index(query, 100, checks = params["checks"]);

print(dist)
output_dist = open("40000_100nndist_mingle_4hct_max1600_16.txt", "w+")
output_result = open("40000_100nnid_mingle_4hct_max1600_16.txt", "w+")

for i in range(1000):
	for j in range(100):
		output_dist.write("%d\n" % dist[i][j])
		output_result.write("%d\n" % result[i][j])






