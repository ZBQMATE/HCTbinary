using namespace std;

void brute_force(bitset<64> *data, bitset<64> *query, int data_len, int z, int &renew_flag, int *ot_score, int *flag_lin, int best_num, int *R, int &len_r) {
	int ot_dst = 0;
	int ot_best_id = 0;
	int ot_cur_best = 64;
	bitset<64> ot_differ;
	if (renew_flag != 0) {
	//if (true) {
		for (int j = 0; j < data_len; j++) {
			ot_score[j] = 64;
		}
		for (int i = 0; i < data_len; i++) {
			if (flag_lin[i] == 0) {
				ot_differ = data[i] ^ query[z];
				for (int d = 0; d < 64; d++) {
					if (ot_differ[d]) {
						ot_dst++;
					}
				}
				ot_score[i] = ot_dst;
				ot_dst = 0;
			}
		}
		for (int n = 0; n < best_num; n++) {
			ot_cur_best = 64;
			ot_best_id = 0;
			for (int nn = 0; nn < data_len; nn++) {
				if (ot_score[nn] < ot_cur_best) {
					ot_best_id = nn;
					ot_cur_best = ot_score[nn];
				}
			}
			ot_score[ot_best_id] = 64;
			checkin(ot_best_id, 0, len_r, R);
		}
	}
}

void hct_build(int assed, int cat, bitset<64> *this_data_idx, int *hct_tree_node, int *hct_bank_tree_node, int *cur_idx, int cur_len, int hct_bf, int hct_leaf_size, int &hct_len_tree_table, int &hct_index_tree_bank) {
	
	if (cur_len < hct_leaf_size) {
		
		cout << "leaf size -- ";
		cout << cur_len << endl;
		
		hct_tree_node[assed * 5 + 0] = 1;
		hct_tree_node[assed * 5 + 1] = cat;
		hct_tree_node[assed * 5 + 2] = hct_index_tree_bank;
		hct_tree_node[assed * 5 + 3] = cur_len;
		hct_tree_node[assed * 5 + 4] = hct_index_tree_bank + cur_len;
		
		for (int i = 0; i < cur_len; i++) {
			hct_bank_tree_node[hct_index_tree_bank + i] = cur_idx[i];
		}
		hct_index_tree_bank = hct_index_tree_bank + cur_len;
		return;
	}
	
	///\\\\\\\\\/////\\\\\////////\\\\\///////
	
	int *rand_bf = new int[hct_bf]();
	int tmp_flag = 0;
	int i = 0;
	for (i = 0; i < hct_bf; i++) {
		int mk_rand = rand() % cur_len;
		for (int j = 0; j < i; j++) {
			if (rand_bf[j] == mk_rand) {
				tmp_flag = 1;
			}
		}
		if (tmp_flag == 0) {
			rand_bf[i] = mk_rand;
		}
		if (tmp_flag == 1) {
			i = i - 1;
		}
		tmp_flag = 0;
	}
	
	// record the centroids
	int *idx_to_data = new int[hct_bf]();
	for (int j = 0; j < hct_bf; j++) {
		idx_to_data[j] = cur_idx[rand_bf[j]];
	}
	int total_len = (1 + cur_len) * hct_bf;
	int *searched_idx = new int[total_len]();
	int *cnt = new int[hct_bf](); // counter of searched_idx
	int cur_best = 64;
	int id_cur_best = 0;
	int dist_a = 0;
	bitset<64> diff_a;
	for (int i = 0; i < cur_len; i++) {
		for (int j = 0; j < hct_bf; j++) {
			diff_a = *(this_data_idx + idx_to_data[j]) ^ *(this_data_idx + cur_idx[i]);
			for (int k = 0; k < 64; k++) {
				if (diff_a[k]) {
					dist_a++;
				}
			}
			if (cur_best > dist_a) {
				cur_best = dist_a;
				id_cur_best = j;
			}
			dist_a = 0;
		}
		searched_idx[id_cur_best * (cur_len + 1) + cnt[id_cur_best]] = i;
		cnt[id_cur_best] = cnt[id_cur_best] + 1;
		id_cur_best = 0;
		cur_best = 64;
	}
	hct_tree_node[assed * 5 + 0] = 0;
	hct_tree_node[assed * 5 + 1] = cat;
	hct_tree_node[assed * 5 + 2] = hct_index_tree_bank;
	hct_tree_node[assed * 5 + 3] = hct_bf;
	hct_tree_node[assed * 5 + 4] = hct_index_tree_bank + hct_bf;
	for (int i = 0; i < hct_bf; i++) {
		hct_bank_tree_node[hct_index_tree_bank + i] = hct_len_tree_table + i;
	}
	hct_index_tree_bank = hct_index_tree_bank + hct_bf;
	int record_tree_len = hct_len_tree_table;
	hct_len_tree_table = hct_len_tree_table + hct_bf;
	for (int i = 0; i < hct_bf; i++) {
		int *t = new int[cnt[i]]();
		for (int j = 0; j < cnt[i]; j++) {
			t[j] = cur_idx[searched_idx[i * (cur_len + 1) + j]];
		}
		hct_build(record_tree_len + i, idx_to_data[i], this_data_idx, hct_tree_node, hct_bank_tree_node, t, cnt[i], hct_bf, hct_leaf_size, hct_len_tree_table, hct_index_tree_bank);
		delete[] t;
	}
	return;
}


void hct_traverse(bitset<64> *this_data_idx, bitset<64> *query_idx, int nnn, int hct_bf, int zzz, int *hct_tree_node, int *hct_bank_tree_node, int *hct_PQ, int *hct_PQ_score, int *hct_R, int *hct_len_pq, int &hct_len_r, int *flag_lin, int ith_tree, int data_len) {
	
	bitset<64> diff_this;
	int cur_best = 64;
	int id_cur_best = 0;
	int tmp_cnt = 0;
	// if bottom layer
	if (hct_tree_node[nnn * 5 + 0] == 1) {
		for (int j = hct_tree_node[nnn * 5 + 2]; j < hct_tree_node[nnn * 5 + 4]; j++) {
			if (flag_lin[hct_bank_tree_node[j]] == 0) {
				hct_R[hct_len_r] = hct_bank_tree_node[j];
				hct_len_r = hct_len_r + 1;
			}
		}
	}
	if (hct_tree_node[nnn * 5 + 0] == 0) {
		// C is the cat set
		int *C = new int[hct_bf]();
		for (int i = 0; i < hct_bf; i++) {
			int id_next = hct_bank_tree_node[hct_tree_node[nnn * 5 + 2] + i];
			C[i] = hct_tree_node[id_next * 5 + 1];
		}
		
		int *score_C = new int[hct_bf]();
		for (int i = 0; i < hct_bf; i++) {
			diff_this = *(this_data_idx + C[i]) ^ *(query_idx + zzz);
			for (int j = 0; j < 64; j++) {
				if (diff_this[j]) {
					tmp_cnt++;
				}
			}
			score_C[i] = tmp_cnt;
			tmp_cnt = 0;
		}
		
		int score_rcd = 64;
		int id_rcd = 0;
		for (int i = 0; i < hct_bf; i++) {
			if (score_C[i] < score_rcd) {
				score_rcd = score_C[i];
				id_rcd = i;
			}
		}
		// cq is the id in hct_tree_node
		int cq = hct_bank_tree_node[hct_tree_node[nnn * 5 + 2] + id_rcd];
		int *pq_rcd = new int[hct_bf - 1]();
		int *pq_score_rcd = new int[hct_bf - 1]();
		int counter_tmp = 0;
		for (int i = 0; i < hct_bf; i++) {
			if (i != id_rcd) {
				pq_rcd[counter_tmp] = hct_bank_tree_node[hct_tree_node[nnn * 5 + 2] + i];
				pq_score_rcd[counter_tmp] = score_C[i];
				counter_tmp++;
			}
		}
		for (int i = 0; i < hct_bf - 1; i++) {
			hct_PQ[ith_tree * data_len + hct_len_pq[ith_tree] + i] = pq_rcd[i];
			hct_PQ_score[ith_tree * data_len + hct_len_pq[ith_tree] + i] = pq_score_rcd[i];
		}
		hct_len_pq[ith_tree] = hct_len_pq[ith_tree] + hct_bf - 1;
		hct_traverse(this_data_idx, query_idx, cq, hct_bf, zzz, hct_tree_node, hct_bank_tree_node, hct_PQ, hct_PQ_score, hct_R, hct_len_pq, hct_len_r, flag_lin, ith_tree, data_len);
	}
	return;
}

void hct_popmin(int *hct_PQ, int *hct_PQ_score, int *hct_len_pq, int num_hct, int data_len, int &poped_id, int &poped_tree) {
	int tmp_best = 64;
	int tmp_id = 0;
	int at_layer = 0;
	
	for (int i = 0; i < num_hct; i++) {
		for (int j = 0; j < hct_len_pq[i]; j++) {
			if (hct_PQ_score[i * data_len + j] <= tmp_best) {
				tmp_best = hct_PQ_score[i * data_len + j];
				tmp_id = j;
				at_layer = i;
			}
		}
	}
	
	poped_id = hct_PQ[at_layer * data_len + tmp_id];
	poped_tree = at_layer;
	
	hct_PQ[at_layer * data_len + tmp_id] = hct_PQ[at_layer * data_len + hct_len_pq[at_layer] - 1];
	hct_PQ_score[at_layer * data_len + tmp_id] = hct_PQ_score[at_layer * data_len + hct_len_pq[at_layer] - 1];
	hct_PQ[at_layer * data_len + hct_len_pq[at_layer] - 1] = 0;
	hct_PQ_score[at_layer * data_len + hct_len_pq[at_layer] - 1] = 0;
	hct_len_pq[at_layer] = hct_len_pq[at_layer] - 1;
	return;
}

void hct(bitset<64> *data, bitset<64> *query, int data_len, int z, int num_hct, int *hct_tree_node, int *hct_bank_tree_node, int *flag_lin, int *hct_PQ, int *hct_PQ_score, int *hct_R, int *hct_len_pq, int &hct_len_r, int *hct_root_id, int &hct_len_tree_table, int &hct_index_tree_bank, int *R, int &len_r) {
	int hct_branch_factor = 16;
	int leaf_size = 300;
	double linear_job = 0.4;
	
	cout << "using hct..." << endl;
	
	if (z == 0) {
		// build the tree
		int *idx_arr = new int[data_len]();
		for (int i = 0; i < data_len; i++) {
			idx_arr[i] = i;
		}
		int tmp_root = 0;
		for (int i = 0; i < num_hct; i++) {
			cout << "build hct..." << endl;
			
			hct_root_id[i] = tmp_root;
			hct_len_tree_table = hct_len_tree_table + 1;
			hct_build(tmp_root, 0, data, hct_tree_node, hct_bank_tree_node, idx_arr, data_len, hct_branch_factor, leaf_size, hct_len_tree_table, hct_index_tree_bank);
			tmp_root = hct_len_tree_table;
		}
	}
	// clear
	for (int i = 0; i < num_hct; i++) {
		for (int j = 0; j < hct_len_pq[i]; j++) {
			hct_PQ[i * data_len + j] = 0;
		}
		for (int j = 0; j < hct_len_r; j++) {
			hct_R[j] = 0;
		}
		hct_len_pq[i] = 0;
		hct_len_r = 0;
	}
	// calculate the number for linear searching
	int total_outlier = 0;
	for (int i = 0; i < data_len; i++) {
		if (flag_lin[i] == 0) {
			total_outlier++;
		}
	}
	int max_exam = int(linear_job * total_outlier);
	int h = 0;
	for (int i = 0; i < num_hct; i++) {
		hct_traverse(data, query, hct_root_id[i], hct_branch_factor, z, hct_tree_node, hct_bank_tree_node, hct_PQ, hct_PQ_score, hct_R, hct_len_pq, hct_len_r, flag_lin, i, data_len);
	}
	
	int poped_id = 0;
	int poped_tree = 0;
	while (hct_len_r < max_exam) {
		hct_popmin(hct_PQ, hct_PQ_score, hct_len_pq, num_hct, data_len, poped_id, poped_tree);
		hct_traverse(data, query, poped_id, hct_branch_factor, z, hct_tree_node, hct_bank_tree_node, hct_PQ, hct_PQ_score, hct_R, hct_len_pq, hct_len_r, flag_lin, poped_tree, data_len);
	}
	for (int i = 0; i < hct_len_r; i++) {
		checkin(hct_R[i], 0, len_r, R);
	}
	return;
}
