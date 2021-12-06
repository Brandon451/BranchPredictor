//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Brandon Saldanha";
const char *studentID   = "A59011109";
const char *email       = "bsaldanha@ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//? Add your own Branch Predictor data structures here

//Gshare
uint8_t *bht_gshare;
uint64_t ghistory;


//------------------------------------//
//        Predictor Functions         //
//------------------------------------//


//Fuction declarations:
void init_gshare();
void train_gshare(uint32_t pc, uint8_t outcome);
uint8_t make_prediction_gshare(uint32_t pc);

// Initialize the predictor
//
void init_predictor()
{
	//? Initialize Branch Predictor Data Structures
	switch (bpType){
		case STATIC:
			break;
		case GSHARE:
			init_gshare();
			break;
		case TOURNAMENT:
			//init_tournament();
			break;
		case CUSTOM:
			//init_custom();
			break;
		default:
			break;
	}

}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc)
{
  	//
  	//TODO: Implement prediction scheme

  	switch (bpType) {
    	case STATIC:
    		return TAKEN;
    	case GSHARE:
			return make_prediction_gshare(pc);
    	case TOURNAMENT:
			//return make_prediction_tournament(pc);
    	case CUSTOM:
			//return make_prediction_custom(pc);
    	default:
    		return NOTTAKEN;
  	}

  	// If there is not a compatable bpType then return NOTTAKEN
  	return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//
void train_predictor(uint32_t pc, uint8_t outcome)
{
  	//
  	//? Implement Predictor training
	switch (bpType){
		case STATIC:
			break;
		case GSHARE:
			train_gshare(pc, outcome);
			break;
		case TOURNAMENT:
			//train_tournament();
			break;
		case CUSTOM:
			//train_custom();
			break;
		default:
			break;
	}
}


//*Gshare functions:

//GSHARE Initialization
void init_gshare(){
	//Allocate array for BHT
	int bht_entries = 1 << ghistoryBits;
	bht_gshare = (uint8_t*)malloc(bht_entries*sizeof(uint8_t));
	for (int i=0; i<bht_entries; i++){
		bht_gshare[i] = WN;
	}
	ghistory = 0;
}

//GSHARE Training
void train_gshare(uint32_t pc, uint8_t outcome){
	uint32_t mask = ~(UINT32_MAX << ghistoryBits);
	uint32_t pcLowerBits = pc & mask;
	uint32_t ghistoryLowerBits = ghistoryBits & mask;
	uint32_t index = ghistoryLowerBits ^ pcLowerBits;

	//Update counters
	switch (bht_gshare[index])
	{
	case SN:
		bht_gshare[index] = (outcome==NOTTAKEN)? SN:WN;
		break;
	case WN:
		bht_gshare[index] = (outcome==NOTTAKEN)? SN:WT;
		break;
	case WT:
		bht_gshare[index] = (outcome==NOTTAKEN)? WN:ST;
		break;
	case ST:
		bht_gshare[index] = (outcome==NOTTAKEN)? WT:ST;
		break;
	default:
		printf("Undefined stats in BHT - Gshare");
		break;
	}

	//Gshare history update
	ghistory = (ghistory<<1) | outcome;
}

//GSHARE Prediction
uint8_t make_prediction_gshare(uint32_t pc){
	uint32_t mask = ~(UINT32_MAX << ghistoryBits);
	uint32_t pcLowerBits = pc & mask;
	uint32_t ghistoryLowerBits = ghistoryBits & mask;
	uint32_t index = ghistoryLowerBits ^ pcLowerBits;

	//Check counter and make predcition
	switch (bht_gshare[index]){
	case SN:
		return NOTTAKEN;
	case WN:
		return NOTTAKEN;
	case WT:
		return TAKEN;
	case ST:
		return TAKEN;
	default:
		printf("Undefined stats in BHT - Gshare");
		return NOTTAKEN;
	}
}

