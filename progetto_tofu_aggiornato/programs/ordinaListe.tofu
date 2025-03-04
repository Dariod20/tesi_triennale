variables
  welcome: string;
  liste, ordinate: [[int]];

functions
  inserisci(n: int, numeri: [int]) -> [int]
    if empty(numeri) then [n]
    else if n <= head(numeri) then
           [n] ++ numeri
         else [head(numeri)] ++ inserisci(n, tail(numeri))
         end
    end;

  ordina(numeri: [int]) -> [int]
    if empty(numeri) then [] 
    else inserisci(head(numeri), ordina(tail(numeri))) 
    end;

  ordinaListe(LL: [[int]]) -> [[int]]
    if empty(LL) then [] 
    else [ordina(head(LL))] ++ ordinaListe(tail(LL)) 
    end;

body
  welcome = "Questo programma ordina liste di liste di numeri\n";
  show welcome;
  show "Inserisci una lista di interi con profondità 2: ";
  liste = ?([[int]]);
  ordinate = ordinaListe(liste);
  show "Ecco la lista inserita riordinata:\n";
  show ordinate;
  
