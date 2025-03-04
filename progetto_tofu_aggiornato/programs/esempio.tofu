variables
	example: string;
	n,x,z:int;
	ins,num:[int];
	ok:[[bool]];
	parole:[string];
	sList:[[[[string]]]];

functions
	inserisci(n:int, numeri: [int]) -> [int]
		if empty(numeri) then [n]
		else if n <= head(numeri) then
			[n] ++ numeri
			else [head(numeri)] ++ inserisci(n, tail(numeri))
			end
		end;
	
	expr(n1:int,n2:int,n3:int)->int
		(n1+n2)-(n3*n3/2)+(-n1);

	max(a:int,b:int)->int
		if a>b then !(a) else !(b) end;

	makeBoolList(x:int,y:int,b1:bool,b2:[bool])->[[bool]]
		[[(x!=y)],[(not b1),(true and b1),((x<y) or false)],tail(b2)++[head(b2)]];
		
body
	example = "Questo è un esempio di programma tofu\n";
	show example;
	x = 21;
	z = 76;
	show "n = ";
	n = max(-100,expr(x,3,12));
	num = [-76,x,37,45];
	ins = inserisci(n,num);
	show "Variabile lista ins = ";
	show ins;
	show "\n";
	ok = makeBoolList(x,z,true,[false,true,true])++[[]];
	show "Variabile lista ok = ";
	show ok;
	show "\n";
	parole = ["cignoNero2022","branco","gregge"];
	sList = [[],[[["cane","gatto"]]],[[[]]],[[parole]++[["stagno"],["rana"]]]++[[["anaconda"]]],[[]]];
	show "Variabile lista sList = ";
	show sList;
	show "\nLa somma delle lunghezze di ok e sList è: ";
	show length(ok)+length(sList);
	show "\nArrivederci";
	
	
	