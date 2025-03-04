variables
    ok: bool;
    input:[string];

functions
    member(elemento: string, lista: [string]) -> bool
    if empty(lista) then false
    else if head(lista) == elemento then true
        else member(elemento, tail(lista))
        end
    end;

    remdup(lista: [string]) -> [string]
    if empty(lista) then []
    else if member(head(lista), tail(lista)) then remdup(tail(lista))
        else [head(lista)] ++ remdup(tail(lista))
        end
    end;

body
    show "Questa funzione rimuove i duplicati lasciando l'ultima occorrenza:\n";
    show "Lista iniziale: ";
    input = ?([string]);
    show "Lista senza doppioni: ";
    show remdup(input);

