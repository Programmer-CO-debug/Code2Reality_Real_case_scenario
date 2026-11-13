// Adventure Quest: The Lost Kingdom
// Console RPG built as per the SRS (C++17, OOP).

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <limits>
#include <algorithm>

using namespace std;

// Thrown when standard input ends (e.g., end of file) so the session can close cleanly.
struct SessionEnded {};

// Reads an integer in [minVal, maxVal]; re-asks on invalid input (FR-12).
int readInt(const string& prompt, int minVal, int maxVal) {
    while (true) {
        cout << prompt;
        int value;
        if (!(cin >> value)) {
            if (cin.eof()) throw SessionEnded{};
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid input. Please enter a number from " << minVal
                 << " to " << maxVal << ".\n";
            continue;
        }
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (value < minVal || value > maxVal) {
            cout << "Invalid choice. Please enter a number from " << minVal
                 << " to " << maxVal << ".\n";
            continue;
        }
        return value;
    }
}

string readLine(const string& prompt) {
    cout << prompt;
    string text;
    if (!getline(cin, text)) throw SessionEnded{};
    return text;
}

// ---------------- Weapon ----------------
class Weapon {
public:
    Weapon(const string& name, int bonus) : name_(name), bonus_(bonus) {}
    const string& getName() const { return name_; }
    int getBonus() const { return bonus_; }
    void display() const {
        cout << "  - " << name_ << " (+" << bonus_ << " attack)\n";
    }
private:
    string name_;
    int bonus_;
};

// ---------------- Enemy ----------------
class Enemy {
public:
    Enemy(const string& name, int health, int attackPower, int xpReward)
        : name_(name), health_(health), attackPower_(attackPower), xpReward_(xpReward) {}

    const string& getName() const { return name_; }
    int getHealth() const { return health_; }
    int getAttackPower() const { return attackPower_; }
    int getXpReward() const { return xpReward_; }
    bool isAlive() const { return health_ > 0; }

    void takeDamage(int amount) {
        health_ = max(0, health_ - amount);
    }

    int attack() const { return attackPower_; }

private:
    string name_;
    int health_;
    int attackPower_;
    int xpReward_;
};

// ---------------- Quest ----------------
class Quest {
public:
    Quest(const string& title, const string& description, int xpReward, const string& rewardWeapon)
        : title_(title), description_(description), xpReward_(xpReward),
          rewardWeapon_(rewardWeapon), completed_(false) {}

    const string& getTitle() const { return title_; }
    int getXpReward() const { return xpReward_; }
    bool isCompleted() const { return completed_; }
    void markCompleted() { completed_ = true; }
    const string& getRewardWeapon() const { return rewardWeapon_; }

    void display(int number) const {
        cout << "  " << number << ". " << title_ << " [" << (completed_ ? "COMPLETED" : "IN PROGRESS") << "]\n"
             << "     " << description_ << "\n"
             << "     Reward: " << xpReward_ << " XP, 25 gold, " << rewardWeapon_ << "\n";
    }

private:
    string title_;
    string description_;
    int xpReward_;
    string rewardWeapon_;
    bool completed_;
};

// ---------------- Character (abstract base) ----------------
class Character {
public:
    Character(const string& name, int health, int baseAttack)
        : name_(name), health_(health), maxHealth_(health), baseAttack_(baseAttack),
          weaponBonus_(0), xp_(0), specialUsed_(false), healUsed_(false) {}

    virtual ~Character() = default;

    // Pure virtual: each hero class defines its own special ability.
    virtual string getClassName() const = 0;
    virtual string getSpecialName() const = 0;
    virtual int specialAbility(Enemy& enemy) = 0;

    // Normal attack: base attack plus equipped weapon bonus.
    virtual int attack(Enemy& enemy) {
        int damage = getAttackPower();
        enemy.takeDamage(damage);
        return damage;
    }

    const string& getName() const { return name_; }
    int getHealth() const { return health_; }
    int getMaxHealth() const { return maxHealth_; }
    int getAttackPower() const { return baseAttack_ + weaponBonus_; }
    int getXp() const { return xp_; }
    bool isAlive() const { return health_ > 0; }

    void takeDamage(int amount) { health_ = max(0, health_ - amount); }

    void heal(int amount) {
        health_ = min(maxHealth_, health_ + amount);
    }

    void addXp(int amount) { xp_ += amount; }

    void equipWeapon(int bonus) {
        if (bonus > weaponBonus_) weaponBonus_ = bonus;   // equip the stronger weapon
    }

    bool isSpecialUsed() const { return specialUsed_; }
    void markSpecialUsed() { specialUsed_ = true; }
    bool isHealUsed() const { return healUsed_; }
    void markHealUsed() { healUsed_ = true; }

    // Called at the start of each battle.
    void resetBattleFlags() {
        specialUsed_ = false;
        healUsed_ = false;
    }

    void displayStatus() const {
        cout << "  Name       : " << name_ << "\n"
             << "  Class      : " << getClassName() << "\n"
             << "  Health     : " << health_ << " / " << maxHealth_ << "\n"
             << "  Attack     : " << getAttackPower() << "\n"
             << "  XP         : " << xp_ << "\n";
    }

protected:
    string name_;
    int health_;
    int maxHealth_;
    int baseAttack_;
    int weaponBonus_;
    int xp_;
    bool specialUsed_;
    bool healUsed_;
};

// ---------------- Warrior ----------------
class Warrior : public Character {
public:
    Warrior(const string& name) : Character(name, 150, 18) {}
    string getClassName() const override { return "Warrior"; }
    string getSpecialName() const override { return "Shield Strike"; }
    // Shield Strike: heavy blow dealing 1.5x damage.
    int specialAbility(Enemy& enemy) override {
        int damage = getAttackPower() * 3 / 2;
        enemy.takeDamage(damage);
        return damage;
    }
};

// ---------------- Mage ----------------
class Mage : public Character {
public:
    Mage(const string& name) : Character(name, 90, 22) {}
    string getClassName() const override { return "Mage"; }
    string getSpecialName() const override { return "Arcane Renewal"; }
    // Arcane Renewal: restores health instead of attacking.
    int specialAbility(Enemy&) override {
        int healAmount = 30;
        heal(healAmount);
        return healAmount;
    }
};

// ---------------- Archer ----------------
class Archer : public Character {
public:
    Archer(const string& name) : Character(name, 110, 20) {}
    string getClassName() const override { return "Archer"; }
    string getSpecialName() const override { return "Precision Shot"; }
    // Precision Shot: aimed arrow that deals double damage.
    int specialAbility(Enemy& enemy) override {
        int damage = getAttackPower() * 2;
        enemy.takeDamage(damage);
        return damage;
    }
};

// ---------------- Game ----------------
class Game {
public:
    Game() {
        setupQuests();
    }

    void run() {
        try {
            showIntro();
            createCharacter();
            mainLoop();
        } catch (const SessionEnded&) {
            cout << "\nInput ended. Closing the session.\n";
        }
        showSummary();
    }

private:
    unique_ptr<Character> player_;
    vector<Weapon> inventory_;
    vector<Quest> quests_;
    size_t currentStage_ = 0;
    int gold_ = 0;
    bool gameOver_ = false;
    bool playerDefeated_ = false;

    void setupQuests() {
        quests_.emplace_back("Forest of Whispers",
            "Defeat the Goblin Scout guarding the forest path.", 40, "Iron Sword (+5)");
        quests_.emplace_back("Marsh of Ashes",
            "Defeat the Orc Brute blocking the road to the castle.", 80, "Steel Axe (+8)");
        quests_.emplace_back("Throne of Shadows",
            "Defeat the Dark Knight guarding the Lost Kingdom's throne.", 150, "Crown Blade (+12)");
    }

    Enemy makeEnemy(size_t stage) const {
        if (stage == 0) return Enemy("Goblin Scout", 60, 8, 40);
        if (stage == 1) return Enemy("Orc Brute", 100, 12, 80);
        return Enemy("Dark Knight", 140, 16, 150);
    }

    int weaponBonusFor(size_t stage) const {
        if (stage == 0) return 5;
        if (stage == 1) return 8;
        return 12;
    }

    void showIntro() const {
        cout << "==============================================\n"
             << "        ADVENTURE QUEST: THE LOST KINGDOM\n"
             << "==============================================\n"
             << "Long ago the Kingdom was lost to shadow.\n"
             << "Choose your hero, face three stages of enemies,\n"
             << "and reclaim the throne.\n\n";
    }

    void createCharacter() {
        string name = readLine("Enter your hero's name: ");
        if (name.empty()) name = "Hero";

        cout << "\nChoose your class:\n"
             << "  1. Warrior  - high health, Shield Strike\n"
             << "  2. Mage     - magic user, Arcane Renewal (heals)\n"
             << "  3. Archer   - ranged fighter, Precision Shot\n";
        int choice = readInt("Your choice (1-3): ", 1, 3);

        if (choice == 1) player_ = make_unique<Warrior>(name);
        else if (choice == 2) player_ = make_unique<Mage>(name);
        else player_ = make_unique<Archer>(name);

        cout << "\nWelcome, " << name << " the " << player_->getClassName() << "!\n";
    }

    void mainLoop() {
        while (!gameOver_) {
            cout << "\n----------------- MAIN MENU -----------------\n"
                 << "Stage " << (currentStage_ + 1) << " of " << quests_.size()
                 << ": " << quests_[currentStage_].getTitle() << "\n"
                 << "  1. Fight stage enemy\n"
                 << "  2. Advance to next stage\n"
                 << "  3. View character status\n"
                 << "  4. View inventory\n"
                 << "  5. View quest journal\n"
                 << "  6. Exit game\n";
            int choice = readInt("Your choice (1-6): ", 1, 6);

            switch (choice) {
                case 1: battle(); break;
                case 2: advanceStage(); break;
                case 3: player_->displayStatus(); break;
                case 4: showInventory(); break;
                case 5: showJournal(); break;
                case 6:
                    cout << "You leave the Lost Kingdom for now.\n";
                    gameOver_ = true;
                    break;
            }
        }
    }

    void battle() {
        Quest& quest = quests_[currentStage_];
        if (quest.isCompleted()) {
            cout << "This quest is already completed. Choose 'Advance to next stage'.\n";
            return;
        }

        Enemy enemy = makeEnemy(currentStage_);
        player_->resetBattleFlags();
        cout << "\n*** BATTLE: " << enemy.getName() << " appears! ***\n";

        while (player_->isAlive() && enemy.isAlive()) {
            cout << "\n" << player_->getName() << " HP: " << player_->getHealth()
                 << " / " << player_->getMaxHealth()
                 << "   |   " << enemy.getName() << " HP: " << enemy.getHealth() << "\n"
                 << "  1. Attack\n"
                 << "  2. " << player_->getSpecialName()
                 << (player_->isSpecialUsed() ? " (used)" : " (once per battle)") << "\n"
                 << "  3. Heal potion (+20 HP)"
                 << (player_->isHealUsed() ? " (used)" : " (once per battle)") << "\n";
            int action = readInt("Your action (1-3): ", 1, 3);

            if (action == 1) {
                int dmg = player_->attack(enemy);
                cout << "You attack for " << dmg << " damage.\n";
            } else if (action == 2) {
                if (player_->isSpecialUsed()) {
                    cout << "You have already used your special ability in this battle.\n";
                    continue;
                }
                player_->markSpecialUsed();
                int value = player_->specialAbility(enemy);
                if (player_->getSpecialName() == "Arcane Renewal")
                    cout << "You use " << player_->getSpecialName() << " and restore " << value << " HP.\n";
                else
                    cout << "You use " << player_->getSpecialName() << " for " << value << " damage.\n";
            } else {
                if (player_->isHealUsed()) {
                    cout << "You have already used the healing potion in this battle.\n";
                    continue;
                }
                player_->markHealUsed();
                player_->heal(20);
                cout << "You drink a healing potion and recover 20 HP.\n";
            }

            if (!enemy.isAlive()) break;

            // FR-05: enemy strikes back if it is still alive.
            int enemyDmg = enemy.attack();
            player_->takeDamage(enemyDmg);
            cout << enemy.getName() << " strikes you for " << enemyDmg << " damage.\n";
        }

        if (!player_->isAlive()) {
            cout << "\nYou have been defeated by the " << enemy.getName() << "...\n";
            playerDefeated_ = true;
            gameOver_ = true;
            return;
        }

        cout << "\n*** VICTORY! " << enemy.getName() << " is defeated. ***\n";
        player_->addXp(enemy.getXpReward());
        gold_ += 25;
        quest.markCompleted();
        player_->equipWeapon(weaponBonusFor(currentStage_));
        inventory_.emplace_back(quest.getRewardWeapon().substr(0, quest.getRewardWeapon().find(" (")),
                                weaponBonusFor(currentStage_));
        cout << "You earn " << enemy.getXpReward() << " XP and 25 gold.\n";
        cout << "Found weapon: " << quest.getRewardWeapon() << " (equipped).\n";
    }

    void advanceStage() {
        if (!quests_[currentStage_].isCompleted()) {
            cout << "You cannot advance yet. Complete the current quest first.\n";
            return;
        }
        if (currentStage_ + 1 >= quests_.size()) {
            cout << "You have completed the final quest!\n";
            gameOver_ = true;
            return;
        }
        ++currentStage_;
        cout << "You advance to Stage " << (currentStage_ + 1) << ": "
             << quests_[currentStage_].getTitle() << "\n";
    }

    void showInventory() const {
        cout << "\nInventory:\n";
        if (inventory_.empty()) cout << "  (empty)\n";
        for (const auto& w : inventory_) w.display();
        cout << "Gold: " << gold_ << "\n";
    }

    void showJournal() const {
        cout << "\nQuest Journal:\n";
        for (size_t i = 0; i < quests_.size(); ++i)
            quests_[i].display(static_cast<int>(i) + 1);
    }

    void showSummary() const {
        cout << "\n==============================================\n"
             << "                 SESSION SUMMARY\n"
             << "==============================================\n";
        if (!player_) {
            cout << "No hero was created.\n";
            return;
        }
        player_->displayStatus();
        cout << "  Gold       : " << gold_ << "\n";
        int done = 0;
        for (const auto& q : quests_) if (q.isCompleted()) ++done;
        cout << "  Quests     : " << done << " of " << quests_.size() << " completed\n";
        if (playerDefeated_) cout << "\nThe Lost Kingdom remains in shadow. Game over.\n";
        else if (done == static_cast<int>(quests_.size())) cout << "\nThe Lost Kingdom is reclaimed! Well done, hero.\n";
        else cout << "\nThe journey continues another day.\n";
    }
};

int main() {
    Game game;
    game.run();
    return 0;
}
