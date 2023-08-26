#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
using namespace std;



enum Side {WHITE, BLACK, BLANK};
const char BLACK_SPACE = '-';

//prompt and error messages
const string prompt1 = "What piece would you like to move? (ex. a4)\n", prompt2 = "Where would you like to move it? (ex. a4)\n",
error1 = "There is no piece there to move!\n", error2 = "That is not your piece!\n", error3 = "That move is not available.\n";

struct Coordinate {
	int x = -1;
	int y = -1;
};
struct Piece{
	string name;
	Side side;
	vector<Coordinate> availableMoves; //This is updated when the program starts and after each move. It contains all the coordinates that the piece can move to.
};

struct Space {
	Piece* piece = nullptr; //nullptr indicates an empty space
};

//piece declarations
Piece rw1, nw1, bw1, qw, kw, bw2, nw2, rw2, pw1, pw2, pw3, pw4, pw5, pw6, pw7, pw8,
rb1, nb1, bb1, qb, kb, bb2, nb2, rb2, pb1, pb2, pb3, pb4, pb5, pb6, pb7, pb8; 


//WHITE represented by 0
//BLACK represented by 1
string playerOne = "WHITE";
string playerTwo = "BLACK";
string winner = ""; //no winner while this is blank
int currentPlayer = 0;

//game board
Space board[8][8];

//needed for game logic
bool availableSpaces[2][8][8]; //This is really two different 2D arrays. The first one keeps track of all the spaces that white pieces can move to using booleans. The second one does the same thing for black.
bool inCheck[2]; //Keeps track of whether or not the king is in check.
int kingPositions[2][2]; //Keeps track of the positions of the white and black kings.

//checks if the coordinates inputted are valid
bool validSpaceInput(string s) {
	return (s[0] - 97) >= 0 && (s[0] - 97) <= 7 && (s[1] - 49) >= 0 && (s[1] - 49) <= 7;
}

bool emptySpace(string s) {
	return board[s[0] - 97][s[1] - 49].piece == nullptr;
}

bool moveAvailable(Piece* piece, int dx, int dy) {
	for (int i = 0; i < piece->availableMoves.size(); i++) {
		if (piece->availableMoves[i].x == dx && piece->availableMoves[i].y == dy) {
			return true;
		}
	}
	return false;
}
//checks if a move can be accomplished, returns "" if it can
string validateMove(int sx, int sy, int dx, int dy) {
	Space space = board[sx][sy];
	if (space.piece == nullptr) return error1;
	Piece *srcPiece = space.piece;
	if ((srcPiece->name[1] == 'w' && currentPlayer == 1) || (srcPiece->name[1] == 'b' && currentPlayer == 0)) return error2; // incorrect team
	if (!moveAvailable(srcPiece, dx, dy)) return error3;

	return "";
}
Side checkSpace(int x, int y){
	if (board[x][y].piece == nullptr) return BLANK;
	return board[x][y].piece->side;
}

//checks if x and y are on the board
bool inBounds(int x, int y) {
	return (x >= 0 && x <= 7 && y >= 0 && y <= 7);
}

void addMove(Piece* p, int dx, int dy) {
	//adds a move to the specific piece
	if (p == nullptr) return;
	Coordinate c = {dx, dy};
	p->availableMoves.push_back(c);

	//update the available spaces array
	if (p->side == WHITE) {
		availableSpaces[0][dx][dy] = true;
	}
	else {
		availableSpaces[1][dx][dy] = true;
	}
}
void updatePawnMoves(Piece* p, int x, int y) {
	int cx, cy;
	if (p->side == BLACK) { //look at spaces below
		cx = x;
		cy = y - 1;
		if (checkSpace(cx, cy) == BLANK) addMove(p, cx, cy);
		cy = y - 2;
		if (y == 6 && checkSpace(cx, cy) == BLANK) addMove(p, cx, cy);

		cx = x - 1;
		cy = y - 1;
		if (cx >= 0) {
			Side s1 = checkSpace(cx, cy);
			if (s1 != BLANK && s1 != p->side) addMove(p, cx, cy);
		}

		cx = x + 1;
		if (cx <= 7) {
			Side s2 = checkSpace(cx, cy);
			if (cx <= 6 && s2 != BLANK && s2 != p->side) addMove(p, cx, cy);
		}
	}
	else { //look at spaces above
		cx = x;
		cy = y + 1;
		if (checkSpace(cx, cy) == BLANK) addMove(p, cx, cy);
		cy = y + 2;
		if (y == 1 && checkSpace(cx, cy) == BLANK) addMove(p, cx, cy);

		cx = x - 1;
		cy = y + 1;
		if (cx >= 0) {
			Side s1 = checkSpace(cx, cy);
			if (cx >= 1 && s1 != BLANK && s1 != p->side) addMove(p, cx, cy);
		}
		cx = x + 1;
		cy = y + 1;
		if (cx <= 7) {
			Side s2 = checkSpace(cx, cy);
			if (cx <= 6 && s2 != BLANK && s2 != p->side) addMove(p, cx, cy);
		}
	}
}
void updateRookMoves(Piece *p, int x, int y) {
	//up direction
	int cx = x, cy = y+1;
	Side s1;
	while (cy <= 7) {
		s1 = checkSpace(cx,cy);
		if (s1 != BLANK) {
			if(s1 != p->side) addMove(p, cx, cy);
			break;
		}
		addMove(p, cx, cy);
		cy++;
	}

	//down direction
	cx = x;
	cy = y-1;
	while (cy >= 0) {
		s1 = checkSpace(cx, cy);
		if (s1 != BLANK) {
			if (s1 != p->side) addMove(p, cx, cy);
			break;
		}
		addMove(p, cx, cy);
		cy--;
	}

	//right direction
	cx = x + 1;
	cy = y;
	while (cx <= 7) {
		s1 = checkSpace(cx, cy);
		if (s1 != BLANK) {
			if (s1 != p->side) addMove(p, cx, cy);
			break;
		}
		addMove(p, cx, cy);
		cx++;
	}

	//left direction
	cx = x - 1;
	cy = y;
	while (cx >= 0) {
		s1 = checkSpace(cx, cy);
		if (s1 != BLANK) {
			if (s1 != p->side) addMove(p, cx, cy);
			break;
		}
		addMove(p, cx, cy);
		cx--;
	}

}
void updateBishopMoves(Piece *p, int x, int y) {
	//diagonal right-up
	int cx = x + 1;
	int cy = y + 1;

	Side s1;
	while (cx <= 7 && cy <= 7) {
		s1 = checkSpace(cx,cy);
		if (s1 != BLANK) {
			if (s1 != p->side) addMove(p, cx, cy);
			break;
		}
		addMove(p, cx, cy);
		cx++;
		cy++;
	}

	//diagonal right-down
	cx = x + 1;
	cy = y - 1;
	while (cx <= 7 && cy >= 0) {
		s1 = checkSpace(cx, cy);
		if (s1 != BLANK) {
			if (s1 != p->side) addMove(p, cx, cy);
			break;
		}
		addMove(p, cx, cy);
		cx++;
		cy--;
	}

	//diagonal left-up
	cx = x - 1;
	cy = y + 1;
	while (cx >= 0 && cy <= 7) {
		s1 = checkSpace(cx, cy);
		if (s1 != BLANK) {
			if (s1 != p->side) addMove(p, cx, cy);
			break;
		}
		addMove(p, cx, cy);
		cx--;
		cy++;
	}

	//diagonal left-down
	cx = x - 1;
	cy = y - 1;
	while (cx >= 0 && cy >= 0) {
		s1 = checkSpace(cx, cy);
		if (s1 != BLANK) {
			if (s1 != p->side) addMove(p, cx, cy);
			break;
		}
		addMove(p, cx, cy);
		cx--;
		cy--;
	}

}
void updateKnightMoves(Piece *p, int x, int y) {
	int cx = x - 1, cy = y + 2;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side) addMove(p,cx,cy);
	}
	cy = y - 2;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side) addMove(p, cx, cy);
	}
	cx = x - 2;
	cy = y + 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side) addMove(p, cx, cy);
	}
	cy = y - 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side) addMove(p, cx, cy);
	}
	cx = x + 1;
	cy = y + 2;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side) addMove(p, cx, cy);
	}
	cy = y - 2;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side) addMove(p, cx, cy);
	}
	cx = x + 2;
	cy = y + 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side) addMove(p, cx, cy);
	}
	cy = y - 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side) addMove(p, cx, cy);
	}
}
void updateKingMoves(Piece* p, int x, int y) {
	int opponentID = 1;
	if (p->side == BLACK) opponentID = 0;
	int cx = x + 1, cy = y+1;
	if (inBounds(cx,cy)) {
		if (checkSpace(cx, cy) != p->side && availableSpaces[opponentID][cx][cy] == false) addMove(p, cx, cy);
	}
	cx = x + 1;
	cy = y;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side && availableSpaces[opponentID][cx][cy] == false) addMove(p, cx, cy);
	}
	cx = x + 1;
	cy = y - 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side && availableSpaces[opponentID][cx][cy] == false) addMove(p, cx, cy);
	}
	cx = x;
	cy = y - 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side && availableSpaces[opponentID][cx][cy] == false) addMove(p, cx, cy);
	}
	cx = x - 1;
	cy = y - 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side && availableSpaces[opponentID][cx][cy] == false) addMove(p, cx, cy);
	}
	cx = x - 1;
	cy = y;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side && availableSpaces[opponentID][cx][cy] == false) addMove(p, cx, cy);
	}
	cx = x - 1;
	cy = y + 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side && availableSpaces[opponentID][cx][cy] == false) addMove(p, cx, cy);
	}
	cx = x;
	cy = y + 1;
	if (inBounds(cx, cy)) {
		if (checkSpace(cx, cy) != p->side && availableSpaces[opponentID][cx][cy] == false) addMove(p, cx, cy);
	}
}
void updateAvailableMoves(Piece* p, int x, int y) {
	if (p == nullptr) return; //space is empty
	char type = p->name[0];
	p->availableMoves.clear();
	switch (type) {
		case 'P':
			updatePawnMoves(p,x,y);
			break;
		case 'R':
			updateRookMoves(p,x,y);
			break;
		case 'N':
			updateKnightMoves(p,x,y);
			break;
		case 'B':
			updateBishopMoves(p,x,y);
			break;
		case 'Q':
			updateRookMoves(p,x,y);
			updateBishopMoves(p,x,y);
			break;
		case 'K':
			if (p->side == WHITE) {
				kingPositions[0][0] = x;
				kingPositions[0][1] = y;
			}
			else {
				kingPositions[1][0] = x;
				kingPositions[1][1] = y;
			}
			updateKingMoves(p,x,y);
			break;
		default:
			break;
	}
}
void resetSpaceArrays() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			availableSpaces[0][i][j] = false;
			availableSpaces[1][i][j] = false;
			
		}
	}
}
void updateAllAvailableMoves() {
	resetSpaceArrays();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			updateAvailableMoves(board[i][j].piece, i, j);
		}
	}
}
string move(string s, string d) {
	int sx = s[0] - 97;
	int sy = s[1] - 49;

	int dx = d[0] - 97;
	int dy = d[1] - 49;
	string val = validateMove(sx,sy,dx,dy);
	if (val != "") return val;
	
	//if this code is executed, the move is valid
	char type = board[sx][sy].piece->name[0];
	Side side = board[sx][sy].piece->side;

	board[dx][dy].piece = board[sx][sy].piece;
	board[sx][sy].piece = nullptr;

	return ""; //success
}
void printBoard() {
	//odd rows start with black, even rows with white
	cout << "   " << string(57, '=') << endl;
	for (int r = 7; r >= 0; r--) { //rows 8 -> 1
		for (int h = 0; h < 3; h++) { //height of each row
			h == 1 ? cout << (" " + to_string(r + 1) + " ") : cout << string(3, ' ');
			for (int c = 0; c < 8; c++) { // columns 1 -> 8	
				if (c == 0) cout << '|';
				int remainders = (r % 2) + (c % 2);
				if (h == 1 && board[c][r].piece != nullptr) { //space not empty
					remainders % 2 == 0 ? cout << (string(2, BLACK_SPACE) + board[c][r].piece->name + string(2, BLACK_SPACE) + '|') : cout << (string(2, ' ') + board[c][r].piece->name + string(2, ' ') + '|');
				}else {
					remainders % 2 == 0 ? cout << (string(6, BLACK_SPACE) + '|') : cout << (string(6, ' ') + '|');
				}
			}
			cout << endl;
		}
		if(r != 0){
			cout << "   |======|======|======|======|======|======|======|======|" << endl;
		}
	}
	cout << "   " + string(57, '=') << endl;
	//bottom lettering
	cout << "       a      b      c      d      e      f      g      h" << endl; 
}
void populateBoard() {
	//0,0 is bottom left

	//WHITE side
	rw1 = {"Rw", WHITE};
	board[0][0].piece = &rw1;
	nw1 = { "Nw", WHITE};
	board[1][0].piece = &nw1;
	bw1 = {"Bw", WHITE};
	board[2][0].piece = &bw1;
	qw = { "Qw", WHITE};
	board[3][0].piece = &qw;
	kw = { "Kw", WHITE};
	board[4][0].piece = &kw;
	bw2 = { "Bw", WHITE };
	board[5][0].piece = &bw2;
	nw2 = { "Nw", WHITE };
	board[6][0].piece = &nw2;
	rw2 = { "Rw", WHITE };
	board[7][0].piece = &rw2;

	pw1 = { "Pw", WHITE };
	board[0][1].piece = &pw1;
	pw2 = { "Pw", WHITE };
	board[1][1].piece = &pw2;
	pw3 = { "Pw", WHITE };
	board[2][1].piece = &pw3;
	pw4 = { "Pw", WHITE };
	board[3][1].piece = &pw4;
	pw5 = { "Pw", WHITE };
	board[4][1].piece = &pw5;
	pw6 = { "Pw", WHITE };
	board[5][1].piece = &pw6;
	pw7 = { "Pw", WHITE };
	board[6][1].piece = &pw7;
	pw8 = { "Pw", WHITE };
	board[7][1].piece = &pw8;


	//BLACK side
	rb1 = { "Rb", BLACK};
	board[0][7].piece = &rb1;
	nb1 = {"Nb", BLACK};
	board[1][7].piece = &nb1;
	bb1 = { "Bb", BLACK};
	board[2][7].piece = &bb1;
	qb = { "Qb", BLACK};
	board[3][7].piece = &qb;
	kb = { "Kb", BLACK };
	board[4][7].piece = &kb;
	bb2 = { "Bb", BLACK };
	board[5][7].piece = &bb2;
	nb2 = { "Nb", BLACK };
	board[6][7].piece = &nb2;
	rb2 = { "Rb", BLACK };
	board[7][7].piece = &rb2;


	pb1 = { "Pb", BLACK };
	board[0][6].piece = &pb1;
	pb2 = { "Pb", BLACK };
	board[1][6].piece = &pb2;
	pb3 = { "Pb", BLACK };
	board[2][6].piece = &pb3;
	pb4 = { "Pb", BLACK };
	board[3][6].piece = &pb4;
	pb5 = { "Pb", BLACK };
	board[4][6].piece = &pb5;
	pb6 = { "Pb", BLACK };
	board[5][6].piece = &pb6;
	pb7 = { "Pb", BLACK };
	board[6][6].piece = &pb7;
	pb8 = { "Pb", BLACK };
	board[7][6].piece = &pb8;

	
	//initialize king positions
	kingPositions[0][0] = 4; 
	kingPositions[0][1] = 0;

	kingPositions[1][0] = 4;
	kingPositions[1][1] = 7;
}

int main() {
	populateBoard();
	string src;
	string dest;

	bool valid;
	bool empty;

	updateAllAvailableMoves();
	while (true) {
		system("CLS");
		printBoard(); cout << endl;

		if (inCheck[0] == true) cout << "WHITE is in check." << endl;
		if (inCheck[1] == true) cout << "BLACK is in check." << endl;

		currentPlayer == 0 ? cout << playerOne << "'s turn: " << endl : cout << playerTwo << "'s turn: " << endl;

		Piece *sP, *dP;
		string m = "";
		while (true) {
			
			if (inCheck[0] || inCheck[1]) {
				char c;
				cout << "If this is checkmate, enter 'r' to resign. Otherwise, enter 'c' to continue:\n";
				cin >> c;
				if (c == 'r') {
					if (currentPlayer == 0) winner = playerTwo;
					else winner = playerOne;
					break;
				}
			}
			do {
				cout << m;
				cout << prompt1;
				do {
					cin >> src;
					valid = validSpaceInput(src);
					empty = valid ? emptySpace(src) : false;
					if (!valid)cout << "Invalid input, please try again." << endl << prompt1;
					else if (empty) cout << "That space is empty, please try again." << endl << prompt1;
				} while (!valid || empty);

				cout << prompt2;

				do {
					cin >> dest;
					valid = validSpaceInput(dest);
					if (!valid) cout << "Invalid input, please try again." << endl << prompt2;
				} while (!valid);

				sP = board[src[0] - 97][src[1] - 49].piece;
				dP = board[dest[0] - 97][dest[1] - 49].piece;

				m = move(src, dest);

			} while (m != "");



			updateAllAvailableMoves();
			inCheck[0] = availableSpaces[1][kingPositions[0][0]][kingPositions[0][1]];
			inCheck[1] = availableSpaces[0][kingPositions[1][0]][kingPositions[1][1]];

			if ((currentPlayer == 0 && inCheck[0]) || (currentPlayer == 1 && inCheck[1])) {
				//move put your king in check, so it is invalid
				//reverse the move
				board[src[0] - 97][src[1] - 49].piece = sP;
				board[dest[0] - 97][dest[1] - 49].piece = dP;
				
				updateAllAvailableMoves();
				cout << "That would put your king in check! Please try again." << endl;
			}
			else {
				break;
			}
		}
		
		if (winner != "") {
			cout << winner << " has won the game!" << endl;
			break;
		}
		currentPlayer = 1 - currentPlayer; //switch current player
		

	}

	
	system("pause");
	
	return 0;
}