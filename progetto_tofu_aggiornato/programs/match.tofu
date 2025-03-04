variables
	p:int;
	s,str:string;

functions
	match(msg:string,s:string,i:int,n:int)->string
		if(i == n) then "Il numero inserito è uguale a quello prefissato"
		else match(!(msg),!(s),?(int),n)
		end;

body
	show "Questo programma verifica quando è stato inserito un certo numero fissato\n";
	show "Inserisci il numero prefissato: ";
	p = ?(int);
	s = "Inserisci un numero: ";
	str = "Il numero inserito NON coincide con quello prefissato!";
	show match(str,!(s),?(int),p);