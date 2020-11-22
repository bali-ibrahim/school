struct Argument {
	int a;
	int b;
};


program CALCULATOR_PROG{
	version CALCULATOR_VERS{
		int ADD(Argument)=1;
		int SUBTRACT(Argument)=2;
		int MULTIPLY(Argument)=3;
		int DIVIDE(Argument)=4;
	}=1;
}=0x77777777;

