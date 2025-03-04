variables
    current: int;

functions
    divides(a: int, b: int) -> bool
        a/b*b == a;

    primeRecursive(n: int, i: int) -> bool
        if n <= 2 or divides(n, i) or i*i > n then
            if n == 2 then
                true
            else
                not divides(n, i)
            end
        else
            primeRecursive(n, i+1)
        end;

    naivePrime(n: int) -> bool
        primeRecursive(n, 2);

    nextPrime(n: int) -> int
        if naivePrime(n) then
            n
        else
            nextPrime(n+1)
        end;

body
    show "Questo programma trova il numero primo più prossimo, ma più grande, dell'intero inserito\n";
    show "Inserisci un numero: ";
    current = ?(int);
    show "\nIl prossimo numero primo >= a quello inserito è: ";
    show nextPrime(current);
    show "\n";