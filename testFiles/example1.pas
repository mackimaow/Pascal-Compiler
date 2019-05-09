program example(input, output);
	var x, y :  integer;
	var z: real; (*
	Temp temp temp *)
	//asdlkansd'l a;s dak
	var temp :  array [ 3 .. 5 ] of integer ;
	var temp2 : array [ 4 .. 5 ] of integer ;
	function gcd(a, b: integer): integer;
	begin
	   if b = 0 then gcd := a
	   else gcd := gcd(b, a mod b)
	end;
	procedure other(thing : array [ 3 .. 5 ] of integer; otherThing : integer );
	begin
		thing[0] := 1 mod 10 / 4 
	end;

begin
	while 0=0 do begin end;
	for y := 1 to 5 do begin end;
	if 0=0 then
	begin
	end;
	// read;
	other(temp, y);
	y:=x;
	// read(x);
	// write(x);
	write(gcd(x, y))
end.
