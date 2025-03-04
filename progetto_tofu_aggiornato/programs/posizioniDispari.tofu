variables
    risultato: [int];
functions 

    shrink(lista: [int]) -> [int]
    if empty(lista) then []
    else if empty(tail(lista)) then lista
        else [head(lista)] ++ shrink(tail(tail(lista)))
        end
    end;

body
    show "Questo programma computa gli elementi di una lista che si trovano in posizione dispari\n";
    risultato = shrink([0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16]);
    show "La Lista risultante è ";
    show risultato;