#include "controller.h"
#include <time.h>
#include <stack>
#include <string>
#include <algorithm>
#include <Windows.h>


using namespace std;



int main()
{
	//PIPING DATA TO WINDOWS FORM
	system("mkdir C:\\Pand");
	std::ofstream("C:\\Pand\\hfile.txt", std::ios::out).close();
	std::ofstream("C:\\Pand\\citydetails.txt", std::ios::out).close();
	fstream tempfile;
	tempfile.open("C:\\Pand\\hfile.txt");
	tempfile << "Name -1 -1 -1 -1 -1 -1 -1 -1 -1 0 ";
	tempfile.close();
	//PIPING DONE
	


	
	int numberofplayers;
	string player[4];
	PandModel GameInstance;//start game
	PandView Screens(GameInstance);//need to copy current state of game
	//after each action, Screens will need to be updated to show current state of game

	//Added this part for role actions ./////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	char roles[7] = { 'a', 'a', 'a', 'a', 'a', 'a', 'a' };   //To keep a track of roles that are still available
	string rolenames[7] = { "Contingency Plan", "Dispatcher", "Medic", "Operations Expert", "Quarantine Specialist", "Researcher", "Scientist" }; //RoleNames


	// Opening introduction and asking for number of players
	Screens.showIntro();

	
	int loadfile, savefile;
	char yn;

	// New Game or Load Game
	int opt;


	cin >> opt;

	while ((opt < 0) || (opt >2)){
		cout << "Invalid Input. Must be 1 or 2." << std::endl;
		cin >> opt;
	}

	if (opt == 2){ //Load

		load:
		system("CLS");
		for (int i = 0; i < 6; i++){
			cout << i + 1 << " - " << "Slot:" << GameInstance.GetAvail(i) << "	Game Name:" << GameInstance.GetGameName(i) << std::endl;
		}
		cout << "Chose a file to load from:";
		cin >> loadfile;

		while ((loadfile < 0) || (loadfile >6) || (GameInstance.GetAvail(loadfile - 1) == "Empty")){
			cout << "Invalid Input or Empty File. Can not Load." << std::endl;
			cin >> loadfile;
		}

		GameInstance.Load(loadfile);
		system("CLS");
		cout << "Game " << GameInstance.GetGameName(loadfile - 1) << " has been loaded from the file!" << std::endl;
		goto start;
	}
	

	system("CLS");
	Screens.askPlayerNumber();

	// Number of players input and validation
	bool validPlayer;
	validPlayer = false;

	while(!validPlayer)
	{
		char num[256];
		std::cout<<"Enter how many players: ";
		std::cin >> num;//this is being skipped the second time around for some reason.
		numberofplayers = atoi(num);
        if (numberofplayers < 2 )
                {
                        cout << "There must be at least 2 players!" << endl;
                }
        else if (numberofplayers > 4)
                {
                        cout << "There cannont be more than 4 players!" << endl;
                }
		if(numberofplayers >= 2 && numberofplayers <= 4) //if 2 or higher and 4 or less, approve
		{validPlayer = true;}
	}

  
	GameInstance.setnumberOfPlayers(numberofplayers);
	GameInstance.assignRoles();//Assigns random roles

	for(int i = 0; i < numberofplayers; i++) 
	{
		cout << "Please enter the name of player " <<i+1<< endl;
		cin >> player[i];
		GameInstance.setPlayerName(i,player[i]);//set player names
	}
	////////////////////////////////////////////////////////////////////////


start:
	int charnum; //temp variable
	charnum = GameInstance.getTurn();






	//Starting hand //need to check for epidemic cards
	for(int i = 0; i < numberofplayers; i++) 
	{
		int numofCards;
		playerCard tempCard;
		if(numberofplayers == 2) numofCards = 4;//2 players get 4 cards at start
		if(numberofplayers == 3) numofCards = 3;//3 players get 3 cards at start
		if(numberofplayers == 4) numofCards = 2;//4 players get 2 cards at start
		for (int j = 0; j<numofCards; j++)
		{
			tempCard = GameInstance.drawPlayerCard();//will draw from the shuffled player cards deck
			if(tempCard.value >47 && tempCard.value < 54)
			{
				j--;//epidemic card, do not store//
				//will need to reinsert epidemic card if drawn
			}
			else//there's a bug here when there are 4 players, hand is incorrect
				GameInstance.ReceiveCard(i,tempCard);//need to modify RecieveCard to take in an object of playercard. and store it on hand
		}
	}

	/////Hardcoded Cards for testing

	/*  Please do not remove /////////////////
	playerCard TESTcard;
	TESTcard.value = 55; //Testing 55 = Forecast
	TESTcard.cardDescription = "ForecastTest";
	GameInstance.ReceiveCard(1, TESTcard);

	playerCard TESTcard2;
	TESTcard2.value = 56;
	TESTcard2.cardDescription = "GovGrantTEST";
	GameInstance.ReceiveCard(1, TESTcard2);
	*/
	///////////////////////////////


	//while(game not ended yet) 
	while(!GameInstance.GameOver() )
	{


		if (GameInstance.getloadflag()){
			//in case we add a mid game load function
			GameInstance.setloadflag(0);
		}
		else{
			GameInstance.setActionsLeft(charnum, 4);//default 4 actions per turn
			} 

		while(GameInstance.getActionsLeft(charnum) != 0 ) //while player is not out of actions
		{
into:
			system("pause");
			system("CLS");
			GameInstance.updateHandsFile(charnum);//used to display in form
			//cout << string(50, '\n'); //Screen clearer.
			cout << "Actions Remaining: " << GameInstance.getActionsLeft(charnum) << endl;

			PandView newScreen(GameInstance);//will refresh when it goes through loop
			Playerchar temp = GameInstance.getPlayerInfo(charnum);
			city tempcity = GameInstance.getCityInfo(temp.location);//integer in getCityInfo should be the int of the city the player is in.
			newScreen.showPlayerInfo(charnum);
			newScreen.showMapInfo();
			newScreen.showCityInfo(temp.location);
			

			newScreen.showActionMenu(temp.profession);
			string ans;
			cin>>ans;


			///////////Adjacent Movement////////////////////
			if(ans == "1")
			{
				cout<<"Where would you like to move :" <<endl;//will move to view.h
				newScreen.showNeighbors(temp.location);
				int moveto;
				cin>>moveto;
				//need a way to determine if the input is valid
				//The amount of cities vary.. should we just assume the player will make the correct input?
				GameInstance.setPlayerLocation(charnum, newScreen.getNeighbor(temp.location, moveto-1));//set player location to new location.
				cout << string(5, '\n');
				cout << "You have moved to " << GameInstance.getCityName(GameInstance.getPlayerLocation(charnum));
				cout << string(5, '\n');
				GameInstance.updateHandsFile(charnum);
			}
			
			/////////////Use Card/////////////////////////////
			else if(ans == "2")
			{
				if(GameInstance.CheckHand(charnum) == 0)
				{
				cout << endl << endl << "You have no cards!" << endl << endl; //Maybe I'll move this to model. -Vu
				GameInstance.setActionsLeft(charnum, 1); //Return action used to Play Card
				}
				if(GameInstance.CheckHand(charnum) == 1) //If player has card, allow to play card
				{GameInstance.PlayCard(charnum);}



			////////EVENT CARD: FORECAST////////////////////
				if(GameInstance.ReturnForecast() == 1)
				{
					cout << "Forecast is being played!  Drawing 6 cards..." << endl;
					int cardplace;
					infectionCard rearrange[6];

					for(int i = 0; i < 6; i++) //Draw 6 cards
					{
					infectionCard tempInfectCard = GameInstance.drawInfectionCard();
					PandView newCards(GameInstance);
					rearrange[i] = tempInfectCard;
					}

					int doneYet = 0;

					//Note: I'm using a copy of the cards inside rearrange. Instead of making a new flag
					//I just use .colors as my flag and change it. Game is unaffected.  -Vu
					while(doneYet != 1) //while cards not all placed
					{
						for(int i = 0; i < 6; i++)
						{
							if(rearrange[i].color != -1)
							{cout << i+1 << ". " << "Infection:  " << rearrange[i].cardDescription << endl;}

						}
						cout << "Choose which card to place on top of the deck (1-6): ";
						cin >> cardplace;
						while(cardplace < 1 || cardplace > 6)
						{cout << "Invalid Input!" << endl; cin >> cardplace;}
						cardplace = cardplace -1; //1 to 0 fix
						
						if(rearrange[cardplace].color != -1)
						{
							GameInstance.placeInfectionCard(rearrange[cardplace]);
							rearrange[cardplace].color = -1;
						}
						else{cout << "Card already chosen!" << endl;}

						if(rearrange[0].color == -1 && rearrange[1].color == -1 && rearrange[2].color == -1 && 
							rearrange[3].color == -1 && rearrange[4].color == -1 && rearrange[5].color == -1)
						{doneYet = 1;}
						cout << endl;
					}

					GameInstance.setForecastPlayed(0); //Forecast has been played and ended
					cout << endl << endl;

				}
				//Resilient population
				if(GameInstance.returnResilient())
				{
					//take a card from the discard infect pile and remove from game
					PandView newCards(GameInstance);

					newCards.showDiscardedInfect();
					cout<<"Choose a city to discard from the infection deck \n";
					int chosen;
					cin>>chosen;
					GameInstance.removeDiscardInfect(chosen);//remove a card from infection deck
					GameInstance.setResilientPlayed(false);//reset
				}

				GameInstance.updateHandsFile(charnum);

			}

			else if(ans == "3") // treat city
			{
				char cubecolor;
				cout << "What color cube would you like to treat?(R, G ,B , Y)\n";
				cin >> cubecolor;
				cubecolor = toupper(cubecolor);
				while (cubecolor != 'R' && cubecolor != 'G' && cubecolor != 'B' && cubecolor != 'Y')
				{
					cout << "Please enter valid cube color: R, G, B, or Y\n";
					cin >> cubecolor;
					cubecolor = toupper(cubecolor);
				}
				int type;
				if(cubecolor == 'R') type = 0;
				if(cubecolor == 'G') type = 1;
				if(cubecolor == 'B') type = 2;
				if(cubecolor == 'Y') type = 3;
				GameInstance.treatCity(GameInstance.getPlayerLocation(charnum), type, GameInstance.getPlayerRole(charnum));
				GameInstance.updateHandsFile(charnum);
			}

			else if(ans == "4")//shuttleFlight
			{
				GameInstance.ShuttleFlight(charnum); GameInstance.updateHandsFile(charnum);
			}

			else if(ans == "5")//list city data
			{
				int printchoice;
				cout << "1. Cities only" << endl;
				cout << "2. Infected Cities" << endl;
				cout << "3. Research Centers" << endl;
				cout << endl << "What would you like shown? : ";
				cin >> printchoice;
				cout << endl;
				if(printchoice == 1)
				{newScreen.printCityColumns();}
				if(printchoice == 2)
				{newScreen.showCubeLocations();}
				if(printchoice ==3)
				{newScreen.printResearchCenters();}
				GameInstance.setActionsLeft(charnum,1);
			//	system("pause");
			}

			//research cure
			else if(ans == "6")
			{
				newScreen.showPlayerHand(charnum);
				

				bool researchSuccess = GameInstance.researchCure(charnum);
				if(!researchSuccess)//if research not successful
				{
					GameInstance.setActionsLeft(charnum, 1);//give back 1 action pt.
					cout<<"You do not have enough cards to research a cure \n";
				}
				else
					cout<<"Research successful \n";
				system("pause");
				GameInstance.updateHandsFile(charnum);
			}

			else if(ans == "7") //share knowledge
			{
				GameInstance.ShareKnowledge(charnum,GameInstance.getnumberOfPlayers() );
				GameInstance.updateHandsFile(charnum);
			}

			else if(ans == "8") //play any event card
			{
				GameInstance.allEventCard(GameInstance.getnumberOfPlayers() );
				GameInstance.setActionsLeft(charnum, 1);
			}

			else if (ans == "9")//game save
			{
			save:	
				string sfname;
				system("CLS");
				for (int i = 0; i < 6; i++){
					cout << i + 1 << " - " << "Slot:" << GameInstance.GetAvail(i) << "	Game Name:" << GameInstance.GetGameName(i) << std::endl;
				}
				cout << "Slot Number:";
				cin >> savefile;
				cout << std::endl;

				while ((savefile < 0) || (savefile >6) ){
					cout << "Invalid Input. Can not save." << std::endl;
					cout << "Slot Number:";
					cin >> savefile;
					cout << std::endl;
				}
				if (GameInstance.GetAvail(savefile - 1) == "Used"){
					
					cout << std::endl << "File is already used. Overwrite? (Y/N)";
				ynInput:
					cin >> yn;
					if ((yn == 'y') || (yn == 'Y')){
						cout << std::endl;
						cout << "Name of your game:";
						cin >> sfname;
						cout << std::endl;
						
						//sfname = delSpaces(sfname);
						

						GameInstance.Save(savefile,sfname,charnum);
						system("CLS");
						cout << "GAME HAS BEEN SAVED INTO SLOT " << savefile << "." << std::endl << std::endl;
						goto into;
					}
					else if ((yn == 'n') || (yn == 'N')){
						goto save;
					}
					else{
						cout << std::endl << "Y/N ?";
						goto ynInput;
					}
				}
				else
				{
					cout << std::endl;
					cout << "Name of your game:";
					cin >> sfname;
					cout << std::endl;
					
					//sfname = delSpaces(sfname);
					

					GameInstance.Save(savefile,sfname,charnum);
					system("CLS");
					cout << "GAME HAS BEEN SAVED INTO SLOT " << savefile << "." << std::endl << std::endl;
					goto into;
				}





			}

			else if (ans == "10")
			{
				goto load;
			}
			// Only these role have active abilities: "Contingency Plan", "Dispatcher","Operations Expert","Researcher"

			else if (ans == "a"){ ////////////////////// ===ACTIVE ROLE ABILITES SHOULD BE CALLED HERE
				if (GameInstance.getPlayerRole(charnum) == 0){//"Contingency Plan"

					GameInstance.pickEvent(charnum);




				}
				if (GameInstance.getPlayerRole(charnum) == 1){//"Dispatcher"
					
					for (int i = 0; i < GameInstance.getnumberOfPlayers(); i++){
						if (charnum != i)cout << i << ")" << GameInstance.getPlayerName(i) << " @ " << GameInstance.getCityName(GameInstance.getPlayerLocation(i)) << endl;

					}
					cout << "Chose a player pawn to move:";
					int pick;
					cin >> pick;
					while (pick<0 || pick > GameInstance.getnumberOfPlayers()-1 || pick == charnum){
						cout << endl << "Invalid Entry. Try Again:";
						cin >> pick;
						cout << endl;
					}
					
					for (int i = 0; i < GameInstance.getnumberOfPlayers(); i++){
						if (i != pick){ cout << i << ")" << GameInstance.getCityName(GameInstance.getPlayerLocation(i)) << endl; }

					}

					cout << "Where would you like to move " << GameInstance.getPlayerName(pick)<<"?";
					int target;
					int loc;
					cin >> target;
					while (target<0 || target>GameInstance.getnumberOfPlayers()-1 || target == pick){
						cout << endl << "Invalid Entry. Try Again:";
						cin >> target;
						cout << endl;
					}
					loc = GameInstance.getPlayerLocation(target);
					GameInstance.setPlayerLocation(pick+1, loc);


				}
				if (GameInstance.getPlayerRole(charnum) == 3){//"Operations Expert"
					cout << "1. Build a Research Center" << endl;
					cout << "2. Discard any city card for charter flight" << endl;
					int expertchoice;
					cout << "What would you like to do? :";
					cin >> expertchoice;

					if(expertchoice == 1)
					{
						GameInstance.buildResearchCenter(charnum);
					}

					if(expertchoice == 2)
					{
					//	newScreen.printCityColumns();
						GameInstance.abilityCharterFlight(charnum);
					}


				}
				if (GameInstance.getPlayerRole(charnum) == 5){//"Researcher"

				}
				GameInstance.updateHandsFile(charnum);

			}
			else if (ans == "b"){ ///////////////////// ====AND HERE
				if (GameInstance.getPlayerRole(charnum) == 1){//"Dispatcher"
					for (int i = 0; i < GameInstance.getnumberOfPlayers(); i++){
						if (charnum != i)cout << i << ")" << GameInstance.getPlayerName(i) << " @ " << GameInstance.getCityName(GameInstance.getPlayerLocation(i))<< endl;

					}
					cout << "Chose a player pawn to move:";
					int pick;
					cin >> pick;
					while (pick<0 || pick>GameInstance.getnumberOfPlayers()-1 || pick == charnum){
					cout << endl << "Invalid Entry. Try Again:";
					cin >> pick;
					cout << endl;
					}
					Screens.showNeighbors(GameInstance.getPlayerLocation(pick));
					int moveto;
					Playerchar temploc = GameInstance.getPlayerInfo(pick);
					cin >> moveto;
					GameInstance.setPlayerLocation(pick+1, newScreen.getNeighbor(temploc.location, moveto - 1));//set player location to new location.
				}
				GameInstance.updateHandsFile(charnum);

			}




			////////////Exit Function//////////////////////
			else
			{
				cout << "Input was not valid" << endl;
				GameInstance.setActionsLeft(charnum,1);
			}

			

			GameInstance.cleanHand(charnum);
			GameInstance.setActionsLeft(charnum, -1); //Subtracts one action
			GameInstance.updateHandsFile(charnum);
			
		}//end of player turn
		if(!GameInstance.GameOver())
		{
			//draw 2 player cards
			cout<<"Draw 2 player cards \n";
			for(int i = 0; i < 2; i++)
			{
				playerCard drawn = GameInstance.drawPlayerCard();//draw from deck
				PandView newCards(GameInstance);
				newCards.showPlayCard(drawn);//display play card drawn
				//if epidemic card
				if(drawn.value > 47 && drawn.value < 54)//if epidemic card
					GameInstance.epidemicDrawn();//call epidemic function

				else{
				//else store in hand
				GameInstance.ReceiveCard(charnum, drawn);//draw card and store in hand
				newCards.showHowManyPlayerCardsLeft();
				}
			}
			system("pause");

			if(!GameInstance.ReturnQuietNight())
			{
			//draw 2-4 infection cards. How many drawn depends on infection rate.
			cout<<"Draw " << GameInstance.getInfectionRate() << " infection cards\n";
			for(int i = 0; i < GameInstance.getInfectionRate(); i++)
			{
				infectionCard tempInfectCard = GameInstance.drawInfectionCard();
				PandView newCards(GameInstance);
				newCards.showInfectCard(tempInfectCard);//display infection card drawn
				GameInstance.addDiseaseCubes(tempInfectCard.city, tempInfectCard.color);//if 3 cubes already exist, if so, dont add 3 but cause outbreak. Done in model
			}
			}
			else
			{
				cout<<"One Quiet Night Played \n";
				GameInstance.setQuietNightPlayed(false);//reset
			}
			system("pause");
		}

		//Next Player
		if(charnum != (numberofplayers-1) ) //If last player, go to first player
		{charnum++;} //0
		else if(charnum == numberofplayers-1){charnum = 0;} //go to next player



		
		cout << string(10, '\n');

		//system("CLS");
	}
	PandView gameover(GameInstance);
	gameover.showGameOverResult();//display ending

	system("pause");
	return 0;
}




