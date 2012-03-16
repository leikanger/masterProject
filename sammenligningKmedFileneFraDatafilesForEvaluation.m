
source 'datafiles_for_evaluation/experiments/secondExperiment/halvannenPeriode/100/log_auron__dKN-depol.oct'; 			KN100d=data;
source 'datafiles_for_evaluation/experiments/secondExperiment/halvannenPeriode/100/log_auron__dKN-depolSPIKES.oct'; 	KN100s=data;

source 'datafiles_for_evaluation/log_auron_dKN-depol.oct'; 			KN100dNY=data;
source 'datafiles_for_evaluation/log_auron_dKN-depolSPIKES.oct'; 		KN100sNY=data;
KN100dNY(1:end,1) = KN100dNY(1:end,1).*(KN100d(end,1)/KN100dNY(end,1));
KN100sNY(1:end,1) = KN100sNY(1:end,1).*(KN100d(end,1)/KN100dNY(end,1));

source 'datafiles_for_evaluation/experiments/secondExperiment/halvannenPeriode/1000/log_auron__dSN-depol.oct'; 			SN1000d=data;
source 'datafiles_for_evaluation/experiments/secondExperiment/halvannenPeriode/1000/log_auron__dSN-depolSPIKES.oct'; 	SN1000s=data;
SN1000d(1:end,1) = SN1000d(1:end,1)./10;
SN1000s(1:end,1) = SN1000s(1:end,1)./10;


% source '10e4/log_auron__dSN-depol.oct'; 		SN10e4d=data;
% SN10e4d(1:end,1) = SN10e4d(1:end,1)/100;
% source '10e4/log_auron__dSN-depolSPIKES.oct'; 	SN10e4s=data;
% SN10e4s(1:end,1) = SN10e4s(1:end,1)/100;

%source '1000000/log_auron__dSN-depol.oct'; 		SN10e6d=data;
%SN10e6d(1:end,1) = SN10e6d(1:end,1)/10000;
source 'datafiles_for_evaluation/experiments/secondExperiment/halvannenPeriode/10e6/log_auron__dSN-depolSPIKES.oct'; 	SN10e6s=data;
SN10e6s(1:end,1) = SN10e6s(1:end,1)/10000;
source 'datafiles_for_evaluation/experiments/secondExperiment/halvannenPeriode/10e6/log_auron__dSN-depol.oct'; SN10e6d=data;
SN10e6d(1:end,1) = SN10e6d(1:end,1)/10000;

plot(
	  SN10e6s(1:end,1) 	, 	SN10e6s(1:end,2), 	".k;;"
	, [1:2] 			, 	SN10e6s(1:2,2), 	"k;Simulated Solution;"
	, SN10e6d(1:end,1) 	, 	SN10e6d(1:end,2), 	".k;;"

	,	SN1000d(1:end,1), 	SN1000d(1:end,2), 	"@g;SN, 1000 iterations;" 
	,	SN1000s(1:end,1), 	SN1000s(1:end,2), 	".g;;"
	,	KN100d(1:end,1), 	KN100d(1:end,2), 	"@b;KN, 100 iterations;" 
	,	KN100s(1:end,1), 	0.98*KN100s(1:end,2), 	".b;;"
	,	KN100dNY(1:end,1), 	KN100dNY(1:end,2), 	"or;KN, NY, 100 iterations;" 
	,	KN100sNY(1:end,1), 	1.1*KN100sNY(1:end,2), 	"- -r;;"
	,	KN100sNY(1:end,1), 	1.1*KN100sNY(1:end,2), 	".b;;"
	  
%	, SN10e6d(1:end, 1), SN10e6d(1:end,2), 		".r;;"
%	, SN10e6d(1:2, 1), 	 SN10e6d(1:2,2), 		"r;Simulated Solution;"
	); 

%axis([1 KN100d(end,1)*1.1 0 1.1*max(KN100s)(2)])
%axis([0 155 0 1400])


%print -dpng -color SLETT.png
sleep(10)
