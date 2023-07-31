#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>

class Card {
public:
    enum rank{ACE = 1, TWO, THREE, FOUR, FIVE, SIX, SEVEN, EIGHT, NINE, TEN, JACK, QUEEN, KING};
    enum suit{CLUBS,DIAMONDS,HEARTS,SPADES};
    friend std::ostream& operator<<(std::ostream& os, const Card& aCard);

    Card(rank r = ACE, suit s = SPADES, bool ifu = true);

    //returns the value of a card, 1-11
    int GetValue() const;

    //flips card; if face up, becomes face down and vice versa
    void Flip();
private:
    rank m_Rank;
    suit m_Suit;
    bool m_IsFaceUp;
};

Card::Card(rank r, suit s, bool ifu): m_Rank(r), m_Suit(s), m_IsFaceUp(ifu)
{}

int Card::GetValue() const {
    //if a card is face down, its value is 0
    int value = 0;
    if (m_IsFaceUp) {
        //value is shown on card
        value = m_Rank;
        //value is 10 for face cards
        if (value > 10) {
            value = 10;
        }
    }
    return value;
}

void Card::Flip() {
    m_IsFaceUp = !(m_IsFaceUp);
}

class Hand {
public:
    Hand();
    virtual ~Hand();
    //adds a card to the hand
    void Add(Card* pCard);

    //clears hand of all cards
    void Clear();

    //gets hand totoal value, intelligently treats aces as 1 or 11
    int GetTotal() const;

protected:
    std::vector<Card*> m_Cards;
};

Hand::Hand() {
    m_Cards.reserve(7);
}

Hand::~Hand() {
    Clear();
}

void Hand::Add(Card* pCard) {
    m_Cards.push_back(pCard);
}

void Hand::Clear() {
    std::vector<Card*>::iterator iter = m_Cards.begin();
    for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter) {
        delete *iter;
        *iter = 0;
    }
    //clear vector of points
    m_Cards.clear();
}

int Hand::GetTotal() const{
    //if no cards in hand, return 0
    if (m_Cards.empty()) {
        return 0;
    }
    //if a first card has value of 0, card is face down
    if (m_Cards[0]->GetValue() == 0) {
        return 0;
    }
    //add up total card values, treat each ace as 1
    int total = 0;
    std::vector<Card*>::const_iterator iter;
    for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter) {
        total += (*iter)->GetValue();
    }
    //determine if hand contains an ace
    bool containsAce = false;
    for (iter = m_Cards.begin(); iter != m_Cards.end(); ++iter) {
        if ((*iter)->GetValue() == Card::ACE) {
            containsAce = true;
        }
    }
    //if card contains ace and total is low enough, treat ace as 11
    if (containsAce && total <= 11) {
        //add only 10 since we've already added 1 for the ace
        total += 10;
    }
    return total;
}

class GenericPlayer : public Hand {
    friend std::ostream& operator<<(std::ostream& os, const GenericPlayer& aGenericPlayer);

public:
    GenericPlayer(const std::string& name = "");

    virtual ~GenericPlayer();
    //indictaes whether or not generic player wants to keep hitting
    virtual bool IsHitting() const = 0;

    //returns whether generic player has busted -- has total greater than 21
    bool IsBusted() const;

    //announces that the player has busted
    void Bust() const;

protected:
    std::string m_Name;
};

GenericPlayer::GenericPlayer(const std::string& name) :
    m_Name(name)
{}

GenericPlayer::~GenericPlayer(){}

bool GenericPlayer::IsBusted() const {
    return (GetTotal() > 21);
}

void GenericPlayer::Bust() const {
    std::cout << m_Name << " has busted.\n";
}

class Player : public GenericPlayer {
public: 
    Player(const std::string& name = "");

    virtual ~Player();

    //returns whether or not the player wants another hit
    virtual bool IsHitting() const;

    //announces player wins
    void Win() const;

    //announces that the player loses
    void Lose() const;

    //announcing that the player pushes
    void Push() const;
};

Player::Player(const std::string& name):
    GenericPlayer(name)
{}

Player::~Player(){}

bool Player::IsHitting() const {
    std::cout << m_Name << ", do you want another hit?(y/n): \n";
    char response;
    std::cin >> response;
    return (response == 'y' || response == 'Y');
}

void Player::Win() const
{
    std::cout << m_Name << " wins!\n";
}

void Player::Lose() const {
    std::cout << m_Name << " loses.\n";
}

void Player::Push() const {
    std::cout << m_Name << " pushes. \n";
}

class House : public GenericPlayer {
public:
    House(const std::string& name = "House");

    virtual ~House();

    //indicates whether house is hitting - will always hit on 16 or less
    virtual bool IsHitting() const;

    //flips over first card
    void FlipFirstCard();
};

House::House(const std::string& name):
    GenericPlayer(name)
{}

House::~House(){}

bool House::IsHitting() const {
    return (GetTotal() <= 16);
}

void House::FlipFirstCard() {
    if (!(m_Cards.empty())) {
        m_Cards[0]->Flip();
    }
    else {
        std::cout << "Theres no card to flip.\n";
    }
}

class Deck : public Hand {
public:
    Deck();

    virtual ~Deck();

    //create a standard deck of 52 cards
    void Populate();

    //shuffle cards
    void Shuffle();

    //deal one card to a hand
    void Deal(Hand& aHand);

    //give additional cards to a generic player
    void AdditionalCards(GenericPlayer& aGenericPlayer);
};

Deck::Deck() {
    m_Cards.reserve(52);
    Populate();
}

Deck::~Deck(){}

void Deck::Populate() {
    Clear();
    //create standard deck
    for (int s = Card::CLUBS; s <= Card::SPADES; ++s) {                 //Loops through all possible combinations of cards and casts the int s and r into the proper enum types.
        for (int r = Card::ACE; r <= Card::KING; ++r) {
            Add(new Card(static_cast<Card::rank>(r),                    //type casting is a way of converting a value of one type to another. static_cast changes the argument to the type between the <> 
                static_cast<Card::suit>(s)));
        }
    }
}

void Deck::Shuffle() {
    random_shuffle(m_Cards.begin(), m_Cards.end());
}

void Deck::Deal(Hand& aHand) {
    if (!m_Cards.empty()) {
        aHand.Add(m_Cards.back());
        m_Cards.pop_back();
    }
    else {
        std::cout << "Out of cards. Unable to deal.";
    }
}

void Deck::AdditionalCards(GenericPlayer& aGenericPlayer) {
    std::cout << "\n";
    //Continue to deal a card as long as generic player isnt busted and wants another hit
    while (!(aGenericPlayer.IsBusted()) && aGenericPlayer.IsHitting()) {
        Deal(aGenericPlayer);
        std::cout << aGenericPlayer << "\n";

        if (aGenericPlayer.IsBusted()) {
            aGenericPlayer.Bust();
        }
    }
}

class Game {
public:
    Game(const std::vector<std::string>& names);

    ~Game();

    //plays a game of blackjack
    void Play();
private:
    Deck m_Deck;
    House m_House;
    std::vector<Player> m_Players;
};

Game::Game(const std::vector<std::string>& names) {
    //create a vector of players from a vector of names
    std::vector<std::string>::const_iterator pName;
    for (pName = names.begin(); pName != names.end(); ++pName) {
        m_Players.push_back(Player(*pName));
    }

    //seed the random number generator
    srand(static_cast<unsigned int>(time(0)));
    m_Deck.Populate();
    m_Deck.Shuffle();
}

Game::~Game(){}

void Game::Play() {
    //deal initial 2 cards to everyone
    std::vector<Player>::iterator pPlayer;
    for (int i = 0; i < 2; ++i) {
        for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer) {
            m_Deck.Deal(*pPlayer);
        }
        m_Deck.Deal(m_House);
    }

    //hide house's first card
    m_House.FlipFirstCard();

    //display everyone's hand
    for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer) {
        std::cout << *pPlayer << "\n";
    }
    std::cout << m_House << "\n";

    //deal additional cards to players
    for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer) {
        m_Deck.AdditionalCards(*pPlayer);
    }

    //reveal house's first card
    m_House.FlipFirstCard();
    std::cout << "\n" << m_House;

    //deal additional cards to house
    m_Deck.AdditionalCards(m_House);

    if (m_House.IsBusted()) {
        //everyone still playing wins
        for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer) {
            if (!(pPlayer->IsBusted())) {
                pPlayer->Win();
            }
        }
    }
    else {
        //compare each player still playing to house
        for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer) 
        {
            if (!(pPlayer->IsBusted())) {
                if (pPlayer->GetTotal() > m_House.GetTotal()) {
                    pPlayer->Win();
                }
                else if (pPlayer->GetTotal() < m_House.GetTotal()) {
                    pPlayer->Lose();
                }
                else {
                    pPlayer->Push();
                }
            }
        }
    }
    //remove everyone's cards
    for (pPlayer = m_Players.begin(); pPlayer != m_Players.end(); ++pPlayer) {
        pPlayer->Clear();
    }
    m_House.Clear();
}

//function prototypes
std::ostream& operator<<(std::ostream& os, const Card& aCard);
std::ostream& operator<<(std::ostream& os, const GenericPlayer& aGenericPlayer);

int main()
{
    std::cout << "\t\tWelcome to Blackjack\n\n";

    //Gather players
    int numPlayers = 0;
    while (numPlayers < 1 || numPlayers > 7) {
        std::cout << "How many players? (1-7): ";
        std::cin >> numPlayers;
    }

    //Setting names
    std::vector<std::string> names;
    std::string name;
    for (int i = 0; i < numPlayers; ++i) {
        std::cout << "Enter player name: ";
        std::cin >> name;
        names.push_back(name);
    }
    std::cout << "\n";

    //the game loop
    Game aGame(names);
    char again = 'y';
    while (again != 'n' && again != 'N') {
        aGame.Play();
        std::cout << "\nDo you want to play again? (Y/N): ";
        std::cin >> again;
    }
    system("pause");
}

//overloads << operator so Card can be sent to cout
std::ostream& operator<<(std::ostream& os, const Card& aCard) {
    const std::string RANKS[] = { "0", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K" };
    const std::string SUITS[] = { "c", "d", "h", "s" };

    if (aCard.m_IsFaceUp) {
        os << RANKS[aCard.m_Rank] << SUITS[aCard.m_Suit];
    }
    else
    {
        os << "XX";
    }
    return os;
}

//overloads << operator so a GenericPlayer can be sent to cout
std::ostream& operator<<(std::ostream& os, const GenericPlayer& aGenericPlayer) {
    os << aGenericPlayer.m_Name << ": \t";

    std::vector<Card*>::const_iterator pCard;
    if (!aGenericPlayer.m_Cards.empty()) {
        for (pCard = aGenericPlayer.m_Cards.begin(); pCard != aGenericPlayer.m_Cards.end(); ++pCard) {
            os << *(*pCard) << "\t";
        }
        if (aGenericPlayer.GetTotal() != 0) {
            std::cout << "(" << aGenericPlayer.GetTotal() << ")";
        }
    }
    else {
        os << "<empty>";
    }
    return os;
}
