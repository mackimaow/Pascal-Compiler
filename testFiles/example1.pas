program example(input, output);
	var x, y:  integer;
	var temp : array [4 .. 22] of integer; 

	function largestFibBefore(maxValue : integer) : integer;
		var x, y : integer;
		procedure initFib ;
		begin
			y := 0;
			x := 1
		end;
		procedure fibIteration ;
		begin
			x := x + y;
			y := x - y
		end;
	begin
		initFib;
		while x < maxValue do
			fibIteration;
		largestFibBefore := y
	end;
begin
	read(x, y);
	temp[4] := largestFibBefore(x);
	temp[5] := largestFibBefore(y);
	write(temp[4], temp[5])


end.
