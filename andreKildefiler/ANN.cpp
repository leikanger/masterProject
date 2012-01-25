
#include "ANN.h"
#include "main.h"
#include "../neuroElements/auron.h"

#if 0
template <class C> ANN<C>::ANN(QuadraticMatrix<double> dEdgeMatrix){

		cout<<"Klarer ikkje gjøre den generell, siden K_synapse og s_synapse er spesifikk for K eller S -auron. Gidder ikkje starte på nytt..\n\nAVSLUTTER\n\n";
		exit(0);
	}
#endif

// Spesialisering for KANN
#if 0
// TODO TODO TODO TODO Gjør denne om, slik at den har en default-vector (som er [0 0 .. 0], med like mange element som getDim dEdgeMatrix!
template <> ANN<K_auron>::ANN(QuadraticMatrix<double> dEdgeMatrix){
		// 1) Lag alle neurona.
		for(int i=0; i<dEdgeMatrix.getDim(); i++){

			// Make Auron name that can be sendt to K_auron::K_auron():
			std::ostringstream tempAuronNavn;
			tempAuronNavn<<"K_auron" <<i;
	
			//std::string tempStr( tempAuronNavn.str() );
	
			// NO from file-open:   // need c-style string for open() function:
			// 						//LogLog_logFile.open( tempStr.c_str() );

			// Construct K_auron:
			addNodeInANN( new K_auron(tempAuronNavn.str() ) );
		}


		// 2) Lag alle synapsene: fra neuron til neuron.
		for(int i1=0; i1<dEdgeMatrix.getDim(); i1++){
			for(int i2=0; i2<dEdgeMatrix.getDim(); i2++){
				if( 	 dEdgeMatrix(i1, i2) > 0){
					// Lag synapse mellom neuron nr. i1 og neuron i2. omega er over null=>exitatoric  synapse (bInhib = false)
					new K_synapse(pAllNodesInANN[i1], pAllNodesInANN[i2], dEdgeMatrix(i1, i2), false); 
					cout<<"Lager synapse: [j,i] " <<i1<<", " <<i2 <<" = " <<dEdgeMatrix(i1, i2) <<endl;
				}else if(dEdgeMatrix(i1, i2) < 0){
					new K_synapse(pAllNodesInANN[i1], pAllNodesInANN[i2], dEdgeMatrix(i1, i2), true); 
				}else{ 
					//matrise-elementet er lik 0: Drit i det!
					continue; //TODO Dette fortsetter til neste iterasjonen på innerste for-løkke?
				}
			}
		}
		cout<<"\nFerdig med å konstruere KANN med kantmatrise:\n";
		dEdgeMatrix.printMatrix();
}
#endif

//template <> ANN<K_auron>::ANN(std::vector<K_auron*> pAuronVector /*= null-vektor*/, QuadraticMatrix<double> dEdgeMatrix){
template <> ANN<K_auron>::ANN(QuadraticMatrix<double> dEdgeMatrix, std::vector<K_auron*> pAuronVector /*= null*/){
	// Default-vector: empty K_auron* vecor. In this case, resize the vector to contain n elements, where n = dim(dEdgeMatrix).
	// 		(empty elements are ignored: new K_auron element is constructed if element nr. i is empty)
	if(pAuronVector.size()==0){
		pAuronVector.resize(dEdgeMatrix.getDim());
	}

	// Check whether pAuronVector has the same lenght as dim(dEdgeMatrix):
	if(pAuronVector.size() != dEdgeMatrix.getDim()){
		cout<<"pAuronVector does not have the same lenght as the dimention of dEdgeMatrix.\nTerminating\n\n(error-ref.: asdf135@ANN.cpp)\n";
		exit(-1);
	}

	cout<<"Vektorlengde, matriseDim: " <<pAuronVector.size() <<", " <<dEdgeMatrix.getDim() <<"\n";

	// 1) Make all aurons that does not appear in pAuronVector as element nr. i:
	for(int i=0; i<dEdgeMatrix.getDim(); i++){

		// Make Auron name that can be sendt to K_auron::K_auron():
		std::ostringstream tempAuronNavn;
		tempAuronNavn<<"K" <<i;

		if(pAuronVector[i]==0){
			cout<<"K_matrix-vector does not contain the " <<i <<"'th element. Construct one.\n";
			addNodeInANN( new K_auron(tempAuronNavn.str() ) );
		}else{
			cout<<"Element nr. " <<i <<" contains an auron pointer. Assigns neuron " <<i <<" of ANN to be this auron.\n";
			addNodeInANN( pAuronVector[i] );
		}
		// Construct K_auron:
	}

	// 2) Lag alle synapsene: fra neuron til neuron.
	for(int i1=0; i1<dEdgeMatrix.getDim(); i1++){
		for(int i2=0; i2<dEdgeMatrix.getDim(); i2++){
			if( 	 dEdgeMatrix(i1, i2) > 0){
				// Lag synapse mellom neuron nr. i1 og neuron i2. omega er over null=>exitatoric  synapse (bInhib = false)
				//new K_synapse(pAllNodesInANN[i1-1], pAllNodesInANN[i2-1], dEdgeMatrix(i1, i2), false);  // XXX 1-indeksert.
				new K_synapse(pAllNodesInANN[i1], pAllNodesInANN[i2], dEdgeMatrix(i1, i2), false); 
				cout<<"Create excitatory synapse: [j,i] " <<i1<<", " <<i2 <<" = " <<dEdgeMatrix(i1, i2) <<endl;
			}else if(dEdgeMatrix(i1, i2) < 0){
				cout<<"Create excitatory synapse: [j,i] " <<i1<<", " <<i2 <<" = " <<dEdgeMatrix(i1, i2) <<endl;
				new K_synapse(pAllNodesInANN[i1], pAllNodesInANN[i2], (dEdgeMatrix(i1, i2)), true); 
			}else{ 
				//matrise-elementet er lik 0: Drit i det!
				continue; //TODO Dette fortsetter til neste iterasjonen på innerste for-løkke?
			}
		}
	}
	cout<<"\nFerdig med å konstruere KANN med kantmatrise:\n";
	dEdgeMatrix.printMatrix();
}



#if 0

// Spesialisering for SANN
template <> ANN<s_auron>::ANN(QuadraticMatrix<double> dEdgeMatrix){
		// 1) Lag alle neurona.
		for(int i=0; i<dEdgeMatrix.getDim(); i++){
			addNodeInANN( new s_auron );
		}

		// 2) Lag alle synapsene: fra neuron til neuron.
		for(int i1=1; i1<=dEdgeMatrix.getDim(); i1++){
			for(int i2=1; i2<=dEdgeMatrix.getDim(); i2++){
				if( 	 dEdgeMatrix(i1, i2) > 0){
					// Lag synapse mellom neuron nr. i1 og neuron i2. omega er over null=>exitatoric  synapse (bInhib = false)
					new s_synapse(pAllNodesInANN[i1], pAllNodesInANN[i2], dEdgeMatrix(i1, i2), false); 
				}else if(dEdgeMatrix(i1, i2) < 0){
					new s_synapse(pAllNodesInANN[i1], pAllNodesInANN[i2], dEdgeMatrix(i1, i2), true); 
				}else{ 
					//matrise-elementet er lik 0: Drit i det!
					continue; //TODO Dette fortsetter til neste iterasjonen på innerste for-løkke?
				}
				;
			}
		}

		cout<<"\nFerdig med å konstruere SANN med kantmatrise:\n";
		dEdgeMatrix.printMatrix();
}
#endif


// Skriver ut alle synapsene med vekt: Har ikkje laga til ei matrise enda, men no finner den verdiene!
template <> const void ANN<K_auron>::printEdgeMatrix(){

	//cout<<"lager matrise i størrelse " <<pAllNodesInANN.size() <<endl;
	QuadraticMatrix<double> tempMatrix(pAllNodesInANN.size(), "tempW");

	for(int n=0; n<pAllNodesInANN.size(); n++){

		std::list<K_synapse*> liste = pAllNodesInANN[n]->getUtSynapserP();
		for(std::list<K_synapse*>::iterator iter = liste.begin(); iter!= liste.end(); iter++ ){
			K_auron* pPostSynAuron = (*iter)->getPostNodeDendrite()->getElementOfAuron();
			for(int i=0; i<pAllNodesInANN.size(); i++){
				if( pAllNodesInANN[i] == pPostSynAuron ){
					// Dette er funky! Matrise-labels er feil veil: i burde stått først (Jmf. Rolls/Treves-konvensjonen).
					tempMatrix(n, i) = (*iter)->getSynVekt();
					//cout<<"n->i: \t"  <<n <<" "<<i <<", " <<(*iter)->getSynVekt() <<endl; 
					break;
				}
			}
		}
	}
	cout<<"FeRDIG: Resultatmatrise:\n";
	tempMatrix.printMatrix();
}



// vim:fdm=marker:fmr=//{,//}
