// Prédicteur corrélé
#ifndef _CORRELE_H
#define _CORRELE_H

#include <vector>

// Ajout d'une information à la class branch_update à titre d'exemple
class my_update : public branch_update {
	 public:
		  unsigned int index;
};

class my_predictor : public branch_predictor {
	 public:

		  enum STATE {
				ST, // 00
				T,  // 01
				NT, // 10
				SNT // 11
		  };

		  // Constructeur
		  // 2^table_bits entrées de 1 bits
		  my_predictor (unsigned int bits, unsigned int l) :
				table_bits(bits),
				tables(1 << l, std::vector<STATE>(1<<bits, SNT)),
				mask((1 << l) - 1)
		  {
		  }

		  // Calcul de la prédiction
		  branch_update *predict (branch_info & b) {
				bi = b;
				if (b.br_flags & BR_CONDITIONAL) {
					 // Saut conditionnel
					 // Récupération des bits de l'adresse pour indexer la table
					 u.index = (b.address & ((1<<table_bits)-1));
					 // Choix de la direction (la mise à jour se fait dans update
					 u.direction_prediction (state_out(tables[hist][u.index]));
				} else {
					 // Saut inconditionnel, 100% sur que c'est pris !
					 u.direction_prediction (true);
				}
				return &u;
		  }

		  // Mise à jour de la table de prédiction
		  void update (branch_update *u, bool taken) {
				// Saut conditionnel
				// On peut forcer à true ou false pour avoir les extrêmes
				if (bi.br_flags & BR_CONDITIONAL) {
					 int index =  ((my_update*)u)->index;
					 STATE s = tables[hist][index];
					 tables[hist][index] = state_machine(s, taken);
					 hist = mask & ((hist << 1) | taken);
				}
		  }

	 private:

		  my_update u;
		  branch_info bi;
		  unsigned int table_bits;
		  std::vector< std::vector<STATE> > tables;

		  unsigned int hist;
		  unsigned int mask;

		  bool state_out(STATE s)
		  {
				if (s == ST || s == T) {
					 return true;
				} else {
					 return false;
				}
		  }

		  STATE state_machine(STATE s, bool taken)
		  {
				switch (s){
					 case ST:
						  if (taken)
								return ST;
						  else
								return T;
					 case T:
						  if (taken)
								return ST;
						  else
								return SNT;
						  break;
					 case NT:
						  if (taken)
								return ST;
						  else
								return SNT;
						  break;
					 case SNT:
						  if (taken)
								return NT;
						  else
								return SNT;
						  break;
					 default:
						  return SNT;
				}
		  }
};

#endif // _2_BITS_H
// vim:se ts=3:
