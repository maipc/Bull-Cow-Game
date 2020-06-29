// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"

#include "Math/UnrealMathUtility.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

// When the game starts
void UBullCowCartridge::BeginPlay() {
  Super::BeginPlay();

  const FString WordListPath =
      FPaths::ProjectContentDir() / TEXT("WordLists/HiddenWordList.txt");
  FFileHelper::LoadFileToStringArray(WordList, *WordListPath);

  Isograms = GetValidWords();

  InitGame();
}

// When the player hits enter
void UBullCowCartridge::OnInput(const FString& Input) {
  ClearScreen();

  if (!bGameOver) {
    ProcessGuess(Input);
  } else {
    ClearScreen();
    InitGame();
  }
}

void UBullCowCartridge::InitGame() {
  int32 RandomNumber = FMath::RandRange(0, Isograms.Num() - 1);
  HiddenWord = Isograms[RandomNumber];
  Lives = HiddenWord.Len() * 2;
  bGameOver = false;

  PrintLine(TEXT("Welcome to Bull Cows"));
  PrintLine(TEXT("Guess the %i letter word!"), HiddenWord.Len());
  PrintLine(TEXT("You have %i lives."), Lives);
  PrintLine(TEXT("Type in your guess. \nPress Enter to continue..."));
}

void UBullCowCartridge::EndGame() {
  bGameOver = true;
  PrintLine(TEXT("Press Enter to play again."));
}

void UBullCowCartridge::ProcessGuess(const FString& Guess) {
  if (Guess == HiddenWord) {
    PrintLine(TEXT("You have Won!"));
    EndGame();
    return;
  }

  Lives--;

  if (!IsIsogram(Guess)) {
    PrintLine(TEXT("No repeating letters!"));
  } else if (Guess.Len() != HiddenWord.Len()) {
    PrintLine(TEXT("The hidden word is %i letter long"), HiddenWord.Len());
  }

  if (Lives == 0) {
    PrintLine(TEXT("You have no lives left!"));
    EndGame();
    return;
  }

  FBullCowCount Score = GetBullCows(Guess);
  PrintLine(TEXT("You have %i Bulls and %i Cows"), Score.Bulls, Score.Cows);
  PrintLine(TEXT("Try again. You have %i lives."), Lives);
}

bool UBullCowCartridge::IsIsogram(const FString& Word) const {
  for (int32 Idx1 = 0; Idx1 < Word.Len(); Idx1++) {
    for (int32 Idx2 = Idx1 + 1; Idx2 < Word.Len(); Idx2++) {
      if (Word[Idx1] == Word[Idx2]) return false;
    }
  }
  return true;
}

TArray<FString> UBullCowCartridge::GetValidWords() const {
  TArray<FString> ValidWords;

  for (FString Word : WordList) {
    if (IsIsogram(Word) && Word.Len() >= 4 && Word.Len() <= 8) {
      ValidWords.Emplace(Word);
    }
  }
  return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const {
  FBullCowCount Count;
  for (int32 Idx = 0; Idx < Guess.Len(); Idx++) {
    if (Guess[Idx] == HiddenWord[Idx]) {
      Count.Bulls++;
      continue;
    }
    for (int32 Idx2 = 0; Idx2 < HiddenWord.Len(); Idx2++) {
      if (Guess[Idx] == HiddenWord[Idx2]) {
        Count.Cows++;
        break;
      }
    }
  }
  return Count;
}