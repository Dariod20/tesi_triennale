variables
    min, max : int;
    lista: [int];

functions
    minimo(lista: [int]) -> int
    if empty(tail(lista)) then head(lista)
    else if head(lista) <= minimo(tail(lista)) then head(lista)
        else minimo(tail(lista))
        end
    end;


    massimo(lista: [int]) -> int
    if empty(tail(lista)) then head(lista)
    else if head(lista) >= massimo(tail(lista)) then head(lista)
        else massimo(tail(lista))
        end
    end;

body
    show "Questo programma indica l'elemento minore e quello maggiore data una lista:\n";
    show "Inserisci una lista di interi: ";
    lista = ?([int]);
    min = minimo(lista);
    show "Il minimo è: ";
    show min;
    show "\n";
    max = massimo(lista);
    show "Il massimo è: ";
    show max;
    show "\n";