variables
	res:int;

functions
	f1(a:int)->int
		if(a > 20) then a
		else f2(a+1)
		end;

	f2(n:int)->int
		if(n < 0) then n
		else f1(n+2)
		end;


body
	show "Questo programma usa funzioni che si richiamano circolarmente: \n";
	show "Inserisci un intero: ";
	res = f1(?(int));
	show "res = ";
	show res;
