variables
    ll: [int];
    elemento: int;

functions
    cancella(lista: [int], elemento: int) -> [int]
    if lista == [] then []
    else if head(lista) == elemento 
        then cancella(tail(lista), elemento)
        else [head(lista)] ++ cancella(tail(lista), elemento)
        end
    end;

body
    show "Questo programma cancella dalla lista un elemento selezionato\n";
    ll = [1,8,2,6,8,138,88,8,3,4,8,18,8,9,6,3,6,7];
    show ll;
    show "\n";
    elemento = 8;
    show cancella(ll, elemento);