#include <Geode/Geode.hpp>
#include <Geode/modify/LevelBrowserLayer.hpp>

using namespace geode::prelude;

namespace FilterManager {
bool filterTiny = false;
bool filterShort = false;
bool filterMedium = false;
bool filterLong = false;
bool filterXL = false;

bool filterVerified = false;
bool filterUnverified = false;

bool anyFilterActive() {
    return filterTiny || filterShort || filterMedium || filterLong ||
           filterXL || filterVerified || filterUnverified;
}

bool levelMatchesFilters(GJGameLevel *level) {
    if (!level)
        return false;

    int length = level->m_levelLength;
    bool matchesLength = false;
    bool anyLengthFilter =
        filterTiny || filterShort || filterMedium || filterLong || filterXL;

    if (!anyLengthFilter) {
        matchesLength = true;
    } else {
        if (length == 0 && filterTiny)
            matchesLength = true;
        if (length == 1 && filterShort)
            matchesLength = true;
        if (length == 2 && filterMedium)
            matchesLength = true;
        if (length == 3 && filterLong)
            matchesLength = true;
        if (length == 4 && filterXL)
            matchesLength = true;
    }

    bool isVerified = level->m_isVerified;
    bool matchesVerification = false;
    bool anyVerificationFilter = filterVerified || filterUnverified;

    if (!anyVerificationFilter) {
        matchesVerification = true;
    } else {
        if (isVerified && filterVerified)
            matchesVerification = true;
        if (!isVerified && filterUnverified)
            matchesVerification = true;
    }

    return matchesLength && matchesVerification;
}
} // namespace FilterManager

CCMenuItemToggler *createTextToggler(const char *text, int tag,
                                     CCObject *target,
                                     SEL_MenuHandler callback) {
    auto labelOn = CCLabelBMFont::create(text, "bigFont.fnt");
    auto labelOff = CCLabelBMFont::create(text, "bigFont.fnt");
    labelOff->setColor({125, 125, 125});

    labelOn->setScale(.6f);
    labelOff->setScale(.6f);

    auto toggler =
        CCMenuItemToggler::create(labelOff, labelOn, target, callback);
    toggler->setTag(tag);
    return toggler;
}

class FilterPopup : public geode::Popup {
  protected:
    bool init() {
        if (!Popup::init(360.f, 125.f))
            return false;

        this->setTitle("Select Filters");

        auto lengthBg = NineSlice::create("square02b_001.png", {0, 0, 80, 80});
        lengthBg->setColor({123, 60, 31});
        lengthBg->setContentSize({340, 30});
        m_mainLayer->addChildAtPosition(lengthBg, Anchor::Center, {0, -30});

        auto lengthBtnMenu = CCMenu::create();
        lengthBtnMenu->setLayout(RowLayout::create()->setGap(15.f));
        lengthBtnMenu->setContentSize({340, 30});
        m_mainLayer->addChildAtPosition(lengthBtnMenu, Anchor::Center,
                                        {0, -30});

        auto tinyTglr = createTextToggler("Tiny", 1, this,
                                          menu_selector(FilterPopup::onToggle));
        tinyTglr->toggle(FilterManager::filterTiny);
        lengthBtnMenu->addChild(tinyTglr);

        auto shortTglr = createTextToggler(
            "Short", 2, this, menu_selector(FilterPopup::onToggle));
        shortTglr->toggle(FilterManager::filterShort);
        lengthBtnMenu->addChild(shortTglr);

        auto mediumTglr = createTextToggler(
            "Medium", 3, this, menu_selector(FilterPopup::onToggle));
        mediumTglr->toggle(FilterManager::filterMedium);
        lengthBtnMenu->addChild(mediumTglr);

        auto longTglr = createTextToggler("Long", 4, this,
                                          menu_selector(FilterPopup::onToggle));
        longTglr->toggle(FilterManager::filterLong);
        lengthBtnMenu->addChild(longTglr);

        auto xlTglr = createTextToggler("XL", 5, this,
                                        menu_selector(FilterPopup::onToggle));
        xlTglr->toggle(FilterManager::filterXL);
        lengthBtnMenu->addChild(xlTglr);

        auto verifiedBg =
            NineSlice::create("square02b_001.png", {0, 0, 80, 80});
        verifiedBg->setColor({123, 60, 31});
        verifiedBg->setContentSize({225, 30});
        m_mainLayer->addChildAtPosition(verifiedBg, Anchor::Center, {0, 10});

        auto verifiedBtnMenu = CCMenu::create();
        verifiedBtnMenu->setLayout(RowLayout::create()->setGap(15.f));
        verifiedBtnMenu->setContentSize({225, 30});
        m_mainLayer->addChildAtPosition(verifiedBtnMenu, Anchor::Center,
                                        {0, 10});

        auto verifiedTglr = createTextToggler(
            "Verified", 6, this, menu_selector(FilterPopup::onToggle));
        verifiedTglr->toggle(FilterManager::filterVerified);
        verifiedBtnMenu->addChild(verifiedTglr);

        auto unverifiedTglr = createTextToggler(
            "Unverified", 7, this, menu_selector(FilterPopup::onToggle));
        unverifiedTglr->toggle(FilterManager::filterUnverified);
        verifiedBtnMenu->addChild(unverifiedTglr);

        lengthBtnMenu->updateLayout();
        verifiedBtnMenu->updateLayout();

        return true;
    }

    void onToggle(CCObject *sender) {
        auto toggler = typeinfo_cast<CCMenuItemToggler *>(sender);
        if (!toggler)
            return;

        bool active = !toggler->isOn();
        int tag = toggler->getTag();

        switch (tag) {
        case 1:
            FilterManager::filterTiny = active;
            break;
        case 2:
            FilterManager::filterShort = active;
            break;
        case 3:
            FilterManager::filterMedium = active;
            break;
        case 4:
            FilterManager::filterLong = active;
            break;
        case 5:
            FilterManager::filterXL = active;
            break;
        case 6:
            FilterManager::filterVerified = active;
            break;
        case 7:
            FilterManager::filterUnverified = active;
            break;
        }
    }

    void onClose(CCObject *sender) {
        Popup::onClose(sender);

        auto runningScene = CCDirector::sharedDirector()->getRunningScene();
        if (auto browser = runningScene->getChildByType<LevelBrowserLayer>(0)) {
            browser->m_searchObject->m_page = 0;
            browser->loadPage(browser->m_searchObject);
        }
    }

  public:
    static FilterPopup *create() {
        auto ret = new FilterPopup();
        if (ret && ret->init()) {
            ret->autorelease();
            return ret;
        }
        CC_SAFE_DELETE(ret);
        return nullptr;
    }
};

template <typename T> struct ScopedRestore {
    T &target;
    T original;

    ScopedRestore(T &target, T temp) : target(target), original(target) {
        target = temp;
    }

    ~ScopedRestore() { target = original; }

    ScopedRestore(const ScopedRestore &) = delete;
    ScopedRestore &operator=(const ScopedRestore &) = delete;
};

class $modify(ModLevelBrowserLayer, LevelBrowserLayer) {
    void loadPage(GJSearchObject *searchObj) {
        if (searchObj && searchObj->m_searchType == SearchType::MyLevels &&
            FilterManager::anyFilterActive()) {

            auto llm = LocalLevelManager::sharedState();
            auto originalArray = llm->m_localLevels;

            if (originalArray) {
                auto filteredArray = CCArray::create();

                for (auto level : CCArrayExt<GJGameLevel *>(originalArray)) {
                    if (FilterManager::levelMatchesFilters(level)) {
                        filteredArray->addObject(level);
                    }
                }

                geode::Ref<CCArray> safeFiltered = filteredArray;

                {
                    ScopedRestore<CCArray *> swap(llm->m_localLevels,
                                                  safeFiltered.data());
                    LevelBrowserLayer::loadPage(searchObj);
                }

                return;
            }
        }

        LevelBrowserLayer::loadPage(searchObj);
    }

    bool init(GJSearchObject *object) {
        if (!LevelBrowserLayer::init(object))
            return false;

        if (object && object->m_searchType == SearchType::MyLevels) {
            auto menu = this->getChildByID("search-menu");
            if (menu) {
                auto plusBtnSpr = CCSprite::createWithSpriteFrameName(
                    "GJ_filterIcon_001.png");
                auto plusBtnBtn = CCMenuItemSpriteExtra::create(
                    plusBtnSpr, this,
                    menu_selector(ModLevelBrowserLayer::onClick));

                menu->addChild(plusBtnBtn);
                auto layout = ColumnLayout::create();
                layout->setAxisAlignment(AxisAlignment::Even);
                layout->setAxisReverse(true);
                layout->setGap(10.f);

                menu->setLayout(layout);
                menu->setPositionX(25.f);
                menu->updateLayout();
            }
        }

        return true;
    }

    void onClick(CCObject *sender) { FilterPopup::create()->show(); }
};