void and_test(int a,int b){
	a & b;
}

void sft_test(int a, int b){
	a >> b;
}

void int_test(){
	int a, b, c, d, e, f, g, h;
	a = 0;
	b = 1;
	c = 2;
	d = -1;
	e = -2;
	f = 10000;
	g = -10000;
}

void unsigned_test(){
	unsigned a, b, c, d, e, f, g, h;
	a = 0;
	b = 1;
	c = 2;
	d = -1;
	e = -2;
	f = 10000;
	g = -10000;
}

void pointer_test(){
	int *p = 0;
	if(p){
		return;
	}
}

void other_test(int a, int b){
	if(a < b){
		a++;
	}
}

void loop_test(){
	int i;
	for(i = 0; i < 10; i++){
		
	}
}
int main(){
	return 0;
}
