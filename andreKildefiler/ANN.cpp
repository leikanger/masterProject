
#include "ANN.h"



template <class C> ANN<C>::ANN(QuadraticMatrix<double>& rEdgeMatrix){

		cout<<"Klarer ikkje gjøre den generell, siden K_synapse og s_synapse er spesifikk for K eller S -auron. Gidder ikkje starte på nytt..\n\nAVSLUTTER\n\n";
		exit(0);
	}

// Spesialisering for KANN
template <> ANN<K_auron>::ANN(QuadraticMatrix<double>& rEdgeMatrix){
		// 1) Lag alle neurona.
		for(int i=0; i<rEdgeMatrix.getDim(); i++){
			addNodeInANN( new K_auron );
		}


		// 2) Lag alle synapsene: fra neuron til neuron.
		for(int i1=1; i1<=rEdgeMatrix.getDim(); i1++){
			for(int i2=1; i2<=rEdgeMatrix.getDim(); i2++){
				if( 	 rEdgeMatrix(i1, i2) > 0){
					// Lag synapse mellom neuron nr. i1 og neuron i2. omega er over null=>exitatoric  synapse (bInhib = false)
					new K_synapse(pAllNodesInANN[i1-1], pAllNodesInANN[i2-1], rEdgeMatrix(i1, i2), false); 
					cout<<"Lager synapse: [j,i] " <<i1<<", " <<i2 <<" = " <<rEdgeMatrix(i1, i2) <<endl;
				}else if(rEdgeMatrix(i1, i2) < 0){
					new K_synapse(pAllNodesInANN[i1-1], pAllNodesInANN[i2-1], rEdgeMatrix(i1, i2), true); 
				}else{ 
					//matrise-elementet er lik 0: Drit i det!
					continue; //TODO Dette fortsetter til neste iterasjonen på innerste for-løkke?
				}
			}
		}
		cout<<"\nFerdig med å konstruere KANN med kantmatrise:\n";
		rEdgeMatrix.skrivUt();
}
// Spesialisering for SANN
template <> ANN<s_auron>::ANN(QuadraticMatrix<double>& rEdgeMatrix){
		// 1) Lag alle neurona.
		for(int i=0; i<rEdgeMatrix.getDim(); i++){
			addNodeInANN( new s_auron );
		}

		// 2) Lag alle synapsene: fra neuron til neuron.
		for(int i1=1; i1<=rEdgeMatrix.getDim(); i1++){
			for(int i2=1; i2<=rEdgeMatrix.getDim(); i2++){
				if( 	 rEdgeMatrix(i1, i2) > 0){
					// Lag synapse mellom neuron nr. i1 og neuron i2. omega er over null=>exitatoric  synapse (bInhib = false)
					new s_synapse(pAllNodesInANN[i1], pAllNodesInANN[i2], rEdgeMatrix(i1, i2), false); 
				}else if(rEdgeMatrix(i1, i2) < 0){
					new s_synapse(pAllNodesInANN[i1], pAllNodesInANN[i2], rEdgeMatrix(i1, i2), true); 
				}else{ 
					//matrise-elementet er lik 0: Drit i det!
					continue; //TODO Dette fortsetter til neste iterasjonen på innerste for-løkke?
				}
				;
			}
		}

		cout<<"\nFerdig med å konstruere SANN med kantmatrise:\n";
		rEdgeMatrix.skrivUt();
}



// DETTE funker ikkje enda. No stikker eg heim igjen.
template <> const void ANN<K_auron>::skrivUtKantMatrise(){
		for(int n=0; n<pAllNodesInANN.size(); n++){
		
			for(int m=0; m<pAllNodesInANN.size() ; m++){
//const std::list<K_synapse*> getUtSynapserP(){
				std::list<K_synapse*> liste = pAllNodesInANN[m]->getUtSynapserP();
				for(std::list<K_synapse*>::iterator iter = liste.begin(); iter!= liste.end(); iter++ ){
					
					K_auron* pPostSynAuron = (*iter)->getPostNodeDendrite()->getElementOfAuron();
					for(int i=0; i<pAllNodesInANN.size(); i++){
						if( pAllNodesInANN[i] == pPostSynAuron ){ cout<<"HELL JEAY: \t"  <<i <<" "<<n <<", " <<m <<endl; }
					}
//					cout<<(((*iter)->getPostNodeDentrite())->getElementOfAuron())->slett <<endl;
				}
				//pAllNodesInANN[n]->pUtSynapser[m];
			}
		}
	}



