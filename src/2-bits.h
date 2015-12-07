// Prédicteur 2-bit à 4 états

// Ajout d'une information à la class branch_update à titre d'exemple
class my_update : public branch_update {
public:
   unsigned int index;
};

class my_predictor : public branch_predictor {
public:
   my_update u;
   branch_info bi;
   bool (*table)[2];
   unsigned int table_bits;

   // Constructeur
   // 2^table_bits entrées de 1 bits
   my_predictor (unsigned int bits, unsigned int l) { 
      // Alloue et met à zéro la table
      table = new bool [1<<bits][2]();
      table_bits = bits;
   }

   // Calcul de la prédiction
   branch_update *predict (branch_info & b) {
      bi = b;
      if (b.br_flags & BR_CONDITIONAL) {
         // Saut conditionnel
         // Récupération des bits de l'adresse pour indexer la table
         u.index = (b.address & ((1<<table_bits)-1));
         // Choix de la direction (la mise à jour se fait dans update
         u.direction_prediction (table[u.index][0]);
      } else {
         // Saut inconditionnel, 100% sur que c'est pris !
         u.direction_prediction (true);
      }
      return &u;
   }

   // Mise à jour de la table de prédiction
   void update (branch_update *u, bool taken, unsigned int target) {
      // Saut conditionnel
      // On peut forcer à true ou false pour avoir les extrêmes
      if (bi.br_flags & BR_CONDITIONAL) {
         table[((my_update*)u)->index][0] = taken;
      }
   }

private:
	 enum STATE {
		  ST, // 00
		  T,  // 01
		  NT, // 10
		  SNT // 11
	 };

	 STATE table2state(bool t[]) {
		  if (t[0]) {
				if (t[1]) { // 11
					 return SNT;
				} else { // 10
					 return NT;
				}
		  } else {
				if (t[1]) { // 01
					 return T;
				} else { // 00
					 return ST;
				}
		  }
	 }

	 STATE graphe1(STATE s, bool taken) {
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
		  }
	 }
};
// vim:se ts=3:
