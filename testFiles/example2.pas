program example(input, output);
	var x, y :  integer;
	// var z: real;
	var temp: array [3 .. 10] of integer;

	// function gcd(a, b: integer): integer;
	// begin
	//    if b = 0 then gcd := a;
	//    gcd := gcd(b, a mod b)
	// end;


	function getIndex(index: integer; arrayValue : array [3 .. 10 ] of integer) : integer;
	var indexTemp : integer;
	begin
		for indexTemp := 3 to 10 do
		begin
			arrayValue[indexTemp] := arrayValue[indexTemp] + 1
		end;
	   	getIndex := arrayValue[index]
	end;	

	// procedure gcd2;
	// begin
	// 	function gcd () : integer;
	// 	begin

	// 	end
	// 	// write(x, temp[4], y)
	//    if y = 0 then y := x
	//    else begin
	//    		temp[3] := x;
	//    		x := y;
	//    		y := temp[3] mod y;
	//    		gcd2
	//    end
	//    // if x <> 10 then
	//    // begin
	//    // 		x := x + 1;
	//    // 		write(x);
	//    // 		gcd2
	//    // end
	// end;

begin

	for x := 3 to 10 do
		temp[x] := x;

	y := getIndex(4, temp);
	write(y, temp[4])

	// write(gcd(112, 24))
	// x := 2;
	// temp[4] := 5;
	// x := 112;
	// y := 24;
	// gcd2;
	// write(y)

	// for x := 3 to 5 do
	// begin
	// 	temp[x] := x;
	// 	write(temp[x] + 5)
	// end

	// for x := 3 to 5 do
	// begin
	// 	if (x < 4) or (x = 5) then
	// 	begin
	// 		write(x)
	// 	end
	// 	else
	// 	begin
	// 		write(-x)
	// 	end

	// end;

	// x := 3;
	// while x <= 10 do
	// begin
	// 	temp[x] := x;
	// 	write(temp[x]);
	// 	x := x + 1
	// end


	// write(x)

	//temp[5] := 123;
	//y := 144 * 2;
	//x := y / 72;
	//write(x, temp[5])
end.