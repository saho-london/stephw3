#include	<iostream>
#include	<string>
#include	<vector>
#include	<cmath>
#include	<memory>

using namespace std;

enum Type {NUMBER, PLUS, MINUS, PROD, DEV, PRTHL, PRTHR};
typedef enum Type Type;

struct Token {
	Type type;
	union {
		double number;
	} value;
};

int index;
#define	NUM(x)	((x).value.number)
#define RIGHT(x) ((x).value.oper.right)
#define LEFT(x) ((x).value.oper.left)

Token readNumber(string line) {
	double number = 0.0;
	while (index < line.length() && isdigit(line[index]))
	{
		number = number * 10 + (line[index] - '0');
		index++;
	}
	if (index < line.length() && line[index] == '.')
	{
		index++;
		double keta = 0.1;
		while (index < line.length() && isdigit(line[index]))
		{
			number += (line[index] - '0') * keta;
			keta *= 0.1;
			index++;
		}
	}

	Token token;
	token.type = NUMBER;
	NUM(token) = number;
	return token;
}

Token readPlus(string line) {
	Token token;
	token.type = PLUS;
	index++;
	return token;
}

Token readMinus(string line) {
	Token token;
	token.type = MINUS;
	index++;
	return token;
}

Token readProd(string line) {
	Token token;
	token.type = PROD;
	index++;
	return token;
}

Token readDev(string line) {
	Token token;
	token.type = DEV;
	index++;
	return token;
}

Token readPrthl(string line) {
	Token token;
	token.type = PRTHL;
	index++;
	return token;
}

Token readPrthr(string line) {
	Token token;
	token.type = PRTHR;
	index++;
	return token;
}

vector<Token> tokenize(string line) {
	vector<Token> tokens;
	index = 0;
	Token token;
	while (index < line.length()) {
		if (isdigit(line[index])) {
			token = readNumber(line);
		} else if (line[index] == '+') {
			token = readPlus(line);
		} else if (line[index] == '-') {
			token = readMinus(line);
		} else if (line[index] == '*') {
			token = readProd(line);
		} else if (line[index] == '/') {
			token = readDev(line);
		} else if (line[index] == '(') {
			token = readPrthl(line);
		} else if (line[index] == ')') {
			token = readPrthr(line);
		} else {
			cout << "Invalid character found: " << line[index] << endl;
			exit(1);
		}
		tokens.push_back(token);
	}
	return tokens;
}

//積と商だけ先に計算する
vector<Token> evaluateProdDev(vector<Token> beforevec) {
	vector<Token> aftervec;
	int i;
	for(i = 0; i < beforevec.size(); i++) {
		Token token;
		if (beforevec[i].type == PROD) {
			if (i + 1 > beforevec.size() - 1) {
				cout << "Invalid syntax" << endl;
				exit(1);
			}
			if (beforevec[i + 1].type != NUMBER) {
				cout << "Invalid syntax" << endl;
				exit(1);
			}
			token.type = NUMBER;
			NUM(token) = NUM(aftervec[aftervec.size() - 1]) * NUM(beforevec[i+1]);
			aftervec.pop_back();
			aftervec.push_back(token);
			i++;
		} else if (beforevec[i].type == DEV) {
			if (i + 1 > beforevec.size() - 1) {
				cout << "Invalid syntax" << endl;
				exit(1);
			}
			if (beforevec[i + 1].type != NUMBER) {
				cout << "Invalid syntax" << endl;
				exit(1);
			}
			token.type = NUMBER;
			NUM(token) = NUM(aftervec[aftervec.size() - 1]) / NUM(beforevec[i+1]);
			aftervec.pop_back();
			aftervec.push_back(token);
			i++;
		} else {
			aftervec.push_back(beforevec[i]);
		}
	}
	aftervec.push_back(beforevec[i]);
	return aftervec;
}

double evaluatePlusMinus(vector<Token> tokens) {
	double answer = 0.0;
	auto iter = tokens.begin();
	Token token;
	token.type = PLUS;
	iter = tokens.insert(iter, token); // Insert a dummy '+' token
	index = 1;
	while (index < tokens.size()) {
		if (tokens[index].type == NUMBER) {
			if (tokens[index - 1].type == PLUS) {
				answer += NUM(tokens[index]);
			} else if (tokens[index - 1].type == MINUS) {
				answer -= NUM(tokens[index]);
			} else {
				cout << "Invalid syntax" << endl;
			}
		}
		index++;
	}
	return answer;
}

double evaluate(vector<Token> tokens) {
	return evaluatePlusMinus(evaluateProdDev(tokens));
}

void test(string line, double expectedAnswer) {
	vector<Token> tokens = tokenize(line);
	double actualAnswer = evaluate(tokens);
	if (abs(actualAnswer - expectedAnswer) < 1.0 / (1 << 24)) {
		cout << "PASS! (" << line << " = " << expectedAnswer << ")" << endl;
	} else {
		cout << "FAIL! (" << line << " should be " << expectedAnswer << " but was " << actualAnswer << ")" << endl;
	}
}

//Add more tests to this function :)
void runTest() {
	cout << "==== Test started! ====" << endl;
	test("1+2", 3);			//デフォルト1
	test("1.0+2.1-3", 0.1);		//デフォルト2
	//2項演算子単体テスト
	//整数
	//足し算
	test("1+1", 2);//正+正
	test("3+0", 3);//正+0
	test("0+2", 2);//0+正
	test("0+0", 0);//0+0
	test("-1+2", 1);//負+正=正
	test("-2+2", 0);//負+正=0
	test("-3+2", -1);//負+正=負
	test("-2+0", -2);//負+0
	//引き算
	test("2-1", 1);//正-正=正
	test("1-1", 0);//正-正=0
	test("2-3", -1);//正-正=負
	test("2-0", 2);//正-0
	test("0-2", -2);//0-正
	test("0-0", 0);//0-0
	test("-2-1", -3);//負-正
	test("-1-0", -1);//負-0
	//掛け算
	test("5*3", 15);//正*正
	test("3*0", 0);//正*0
	test("0*1", 0);//0*正
	test("0*0", 0);//0*0
	test("-1*2", -2);//負*正
	test("-1*0", 0);//負*0
	//割り算
	test("4/2", 2);//正/正 割り切れる
	test("3/2", 1.5);//正/正 割り切れない
	//test("4/0", inf); 正/0 test関数がinfに非対応
	test("0/4", 0);//0/正
	//test("0/0", nan); 0/0 test関数がNaNに非対応
	//小数
	//足し算
	test("1.5+1.4", 2.9);//正+正
	test("3.2+0.0", 3.2);//正+0
	test("0.0+2.3", 2.3);//0+正
	test("0.0+0.0", 0.0);//0+0
	test("-1.0+2.3", 1.3);//負+正=正
	test("-2.3+2.3", 0.0);//負+正=0
	test("-3.5+2.3", -1.2);//負+正=負
	test("-2.3+0.0", -2.3);//負+0
	//引き算
	test("2.2-1.0", 1.2);//正-正=正
	test("1.3-1.3", 0.0);//正-正=0
	test("2.3-3.3", -1.0);//正-正=負
	test("2.2-0.0", 2.2);//正-0
	test("0.0-2.2", -2.2);//0-正
	test("0.0-0.0", 0.0);//0-0
	test("-2.2-1.1", -3.3);//負-正
	test("-1.2-0.0", -1.2);//負-0
	//掛け算
	test("1.2*1.2", 1.44);//正*正
	test("3.2*0.0", 0.0);//正*0
	test("0.0*1.2", 0.0);//0*正
	test("0.0*0.0", 0.0);//0*0
	test("-1.2*2.4", -2.88);//負*正
	test("-1.3*0.0", 0.0);//負*0
	//割り算
	test("4.2/2.1", 2.0);//正/正 割り切れる
	test("3.3/2.2", 1.5);//正/正 割り切れない
	//test("4.3/0.0", inf); 正/0 test関数がinfに非対応
	test("0.0/4.2", 0);//0/正
	//test("0.0/0.0", nan); 0/0 test関数がNaNに非対応
	//複合テスト
	//整数
	//和・差
	test("2+3+4", 9);
	test("2+3-4", 1);
	test("2-3+4", 3);
	test("2+0+4", 6);
	test("0+3-4", -1);
	test("2-3+0", -1);
	test("2-3-4", -5); //前方結合
	//積・商
	test("2*3*4", 24);
	test("3*3/2", 4.5);
	test("3/4*2", 1.5);
	test("3/2/2", 0.75); //前方結合
	//小数
	//和・差
	test("2.1+3.2+4.3", 9.6);
	test("2.1+3.2-4.3", 1.0);
	test("2.1-3.2+4.3", 3.2);
	test("2.1+0.0+4.3", 6.4);
	test("0.0+3.2-4.3", -1.1);
	test("2.1-3.2+0.0", -1.1);
	test("2.1-3.2-4.3", -5.4); //前方結合
	//積・商
	test("2.1*3.2*4.3", 28.896);
	test("3.2*3.3/2.2", 4.8);
	test("3.2/4.2*2.1", 1.6);
	test("3.0/2.5/2.5", 0.48); //前方結合
	//なんかいろいろ
	test("1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1+1-1", 0);
	test("2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2*2", 16777216);
	cout << "==== Test finished! ====" << endl;
}

int main()
{
	runTest();
	
	double answer;
	while(true) {
		cout << "> ";
		string line;
		cin >> line;
		vector<Token> tokens = tokenize(line);
		answer = evaluate(tokens);

		cout << "answer = " << answer << endl;
	}

	return 0;
}
