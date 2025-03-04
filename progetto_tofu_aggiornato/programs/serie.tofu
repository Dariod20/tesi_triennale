functions
    serie(n: int) -> [int]
    if n == 0 then []
    else serie(n - 1) ++ [n]
    end;


body
    show "Inserisci un numero per creare la serie: ";
    show serie(?(int));