#include "main.h"
#include <typeinfo>


using std::cout;
using std::endl;
using std::string;

#define MAX_NEURON 256



/************************
** MERK: Både QuadraticMatrix og ANN er 1-indeksert!!!
** XXX XXX XXX XXX XXX XXX
**************************/









template <class C> class QuadraticMatrix{
// ALLTERNATIVT med TEMPLATE: template <class C, unsigned uDim> class QuadraticMatrix{   FUNKER ENDA BEDRE!
	unsigned uDim;
	C m[MAX_NEURON][MAX_NEURON];
	string sNavn;
	public:
		QuadraticMatrix(unsigned uDimArg, string sNavnArg = "QuadM") : uDim(uDimArg), sNavn(sNavnArg)
		{
			for(int i1=0; i1<uDim; i1++){
				for(int i2=0; i2<uDim; i2++){
					m[i1][i2] = 0;
				}
			}
	 		std::cout<<"Lager kvadratisk matrise: " <<uDim <<"X" <<uDim <<"\n";
		}
		

		// Dette funker: kalles f.eks. ved QM(1,2) - n=1,m=2!
		C& operator()(unsigned uN, unsigned uM)
		{
//  /// ///// KOMMENTERT UT: Matrisen er nå 0-indeksert!
//			// Først: gjøre indeks 1-indeksert:
//			uN --;
//			uM --;

			// Look for Out-Of-Bounds-Error
			if(uN >= uDim || uM >= uDim){
				cout<<"\n\nasdf163@ANN.h: matrix-out-of-bounds-ERROR\n"
					<<"(n, m, dim: \t[" <<uN+1 <<", " <<uM+1 <<", " <<uDim <<"])\n"
					<<"((n,m er 1-indeksert.. Hugs: index 0 er også out of bounds!))\nAVSLUTTER!\n\n";
					
				exit(-9);
			}
		
			
			// TODO Bli heilt sikker på kva skrivemåẗe eg skal bruke ([kol. , rad] eller [rad, kol] )
			// (kan bytte ved å bytte plass på uM og uN, under..)
			return m[uN][uM];	
	 
		}

		int getDim() const {
			return uDim;
		}
		const void printMatrix(){
			cout<<"PrintMatrix:\n";
			for(int i=0; i<uDim; i++){
				if(i == (uDim)/2){
					cout<<sNavn 	<<" =";
				}

				for(int i2=0; i2<uDim; i2++){
					cout<<"\t" <<m[i][i2];
				}
				cout<<"\n";
			}
			cout<<"\n";
		}
};


template <class C> class ANN{
	//Liste over alle Kappa auron: 	
	std::vector<C*> pAllNodesInANN;
/* PLAN: ha ei matrise, og kvar gang en kant endres, kan matrisen endres også! */
	//QuadraticMatrix edgeMatrix;


	inline void addNodeInANN(C* pNewNode){
		pAllNodesInANN.push_back(pNewNode);
	}

	// IKKJE I BRUK 
#if 0
	inline void makeEmptyANN(unsigned uDim){
		for(int i=0; i<nAntallNoder; i++){
			//std::cout<<"Lager " <<typeid(C).name() <<" (" <<i <<" av " <<nAntallNoder <<")\n";
			pAllNodesInANN.push_back( new C );
		}
	}
#endif
public:
	/******** Constructor  ***********
	**
	** 	ANN(QuadraticMatrix<double> rEdgeMatrix) 											: 	lager tomme neuron med koblinger som gitt i rEdgeMatrix
	** 	ANN(QuadraticMatrix<K_auron*> rAuronMatrix. QuadraticMatrix<double> rEdgeMatrix) 	: 	lager ANN med aurona peiket til i rAuronMatrix med koblinger som i rEdgeMatrix
	** 																							(dersom rAuronMatrix inneholder 0-pekere, skal nytt K_auron initieres!)
	*********************************/
	//ANN(QuadraticMatrix<double> rEdgeMatrix); //TODO Ta vekk referanse-arg.!
	//ANN(std::vector<C*> pAuronVector, QuadraticMatrix<double> dEdgeMatrix);
	ANN(QuadraticMatrix<double> dEdgeMatrix, std::vector<C*> pAuronVector =std::vector<C*>(0)  );


	const void printEdgeMatrix();



	/* newInputSynapse(C* pNewInputNeuron, float fSynapticWeight){
		TODO Ha med noke slikt for å få inputsynapser? (f.eks. dersom eg skal koble til en input-node. Eller:
			1) Sensory neurons har stort sett ikkje input, og dermed trengs ikkje denne å være med når vi ser på kant-matrise. MEN: det blir mykje lettere å se på den som en del av ANN.
			2) Kan lage sensory neuron eksternt, og lage input-synapser til ANN. Trenger da en mdl-funk som enkelt legger til en synapse til node (gitt av en index (fra matrisa) ).

	*/	

};

// vim:fdm=marker:fmr=//{,//}
