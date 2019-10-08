from selenium import webdriver
import sys
from selenium.webdriver.common.keys import Keys
from threading import Thread
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from bs4 import BeautifulSoup
from selenium.webdriver.support.ui import Select
import time
import re
#=================== PRE PROVIDED CODE. DO NOT TOUCH! ===================#

# Replace username/password with yours
# Do not share your credential with anyone!!!
ACCOUNTS = {
    "username":"password"
}

URL = 'https://paws.tcnj.edu/'
# Login webpage
LOGIN_INI = 'https://paws.tcnj.edu/psp/paws/?cmd=login&languageCd=ENG&'
# After login webpage
# This URL works for both faculty and students
LOGIN_URL = 'https://paws.tcnj.edu/psp/paws/EMPLOYEE/SA/h/?tab=DEFAULT'

#=================== END OF PRE-PROVIDED CODE  ==========================#


def searchCourse(driver, user, programName, courseN):
    # Pace all requests by 2 seconds (PACING OCCURS FOR REST OF CODE)
    time.sleep(2)                   
    print("Finding Student Center")
    studentCenter = driver.find_element_by_xpath('//*[text()="Student Center"]')    # Find the Student center element & click on it
    studentCenter.click()
    print("Student Center link clicked")

    time.sleep(2)
    print("Finding drop down menu")
    dropList = driver.find_element_by_xpath("//*[@id='pthnavbc_CO_EMPLOYEE_SELF_SERVICE']") # Find & click on drop down menu 
    dropList.click()
    print("Drop down menu enabled")

    time.sleep(2)
    print("Finding Class Search link")
    searchElement = driver.find_element_by_id("crefli_HC_CLASS_SEARCH_GBL")     # Find & click on class search after drop down was enabled
    searchElement.click()
    print("Class Search link clicked")

    time.sleep(2)
    print("Switching to correct frame")
    driver.switch_to.frame(driver.find_element_by_name('TargetContent'))    # Switch to iframe that has the search form included
    print("Switched to correct frame")

    time.sleep(2)
    print("Changing to current semester")
    semesterList = Select(driver.find_element_by_xpath('//*[@id="CLASS_SRCH_WRK2_STRM$35$"]')) # Find the drop down for picking semester
    semesterList.select_by_index(1)     # Always select item at index 1 (will always be current sem bc of the order of the drop list)
    print("Changed to current semester")

    time.sleep(2)
    print("Unchecking 'open classes only'")
    checkbox = driver.find_element_by_xpath('//*[@id="SSR_CLSRCH_WRK_SSR_OPEN_ONLY$3"]') # Find the checkbox for open classes
    checkbox.click()                # Click on found checkbox
    print("Unchecked 'open classes only'")

    time.sleep(2)
    print("Selecting Program Name")
    classList = Select(driver.find_element_by_xpath('//*[@id="SSR_CLSRCH_WRK_SUBJECT_SRCH$0"]')) # Find the class list drop down
    try:
        classList.select_by_value(programName.upper())      # Pick the correct value (all the codes are uppercase)
        print("Program Name selected")
    except:
        print("\n" + programName.upper() + " was not found. Sorry :(")
        input("Press Enter to continue...")
        return 0

    time.sleep(2)
    print("Entering Course Number")
    numberInput = driver.find_element_by_xpath('//*[@id="SSR_CLSRCH_WRK_CATALOG_NBR$1"]')   # Find the box for putting in class number
    numberInput.click()                 # Actually click on the box (so keys that are sent are actually inputted)
    numberInput.send_keys(courseN)      # Input the class code
    print("Course Number Entered")

    time.sleep(2)
    print("Submitting search request")
    searchButton = driver.find_element_by_xpath('//*[@id="CLASS_SRCH_WRK2_SSR_PB_CLASS_SRCH"]')     # Find the button for searching
    searchButton.click()                    # Click on that search button
    print("Submitted search request")
    
    time.sleep(4)
    print("\nPrinting results...\n")
    allStatusList = []          # allStatusList array will hold all values for classes (either 'Open' or 'Closed')
    allClassCode = []           # allClassCode array will hold the class number (ex: 83618)
    allTimeInfo = []            # allTimeInfo array will hold class times (ex: TuFri 8:00-11:00)
    allTeacherInfo = []         # allTeacherInfo array will hold who the professor is for sections (professor name)

    # pageContent will hold the results page's HTML so that we can parse through it
    pageContent = BeautifulSoup(driver.page_source, 'html.parser') 

    # pawsCode will hold results of finding all elements with PSHYPERLINK class and Class Nbr title 
    pawsCode = pageContent.find_all(class_="PSHYPERLINK", title="Class Nbr")   

    # statusImage will hold the results from finding the image elements that depict if a class is open/closed (green circle/blue square)
    statusImage = pageContent.find_all(style="min-width:52px; ")

    # timeInfo will hold the results from finding all PSLONGEDITBOX class elements that contain "MTG_DAYTIME" in id, even if that is not full id
    timeInfo = pageContent.find_all(class_="PSLONGEDITBOX", id=re.compile('^MTG_DAYTIME'))

    # teacherInfo will hold the results from finding all PSLONGEDITBOX class elements that contain "MTG_INSTR" in id, even if that is not full id
    teacherInfo = pageContent.find_all(class_="PSLONGEDITBOX", id=re.compile('MTG_INSTR'))

    # for each item in statusImage list, find the 'img' elements
    for item in statusImage:
        classStatus = (item.findChildren('img'))

        # for each 'img' element, find the what the element's alt text is (it is either Open or Closed which will be useful)
        # Append the findings into the allStatusList (previously defined)
        for item in classStatus:
            allStatusList.append(item.get('alt'))

    # This for loop will append the text of pawsCode, timeInfo, and teacherInfo list contents into respective arrays (previously defined)
    for counter in range(0, len(pawsCode)):
        allClassCode.append(pawsCode[counter].get_text())
        allTimeInfo.append(timeInfo[counter].get_text())
        allTeacherInfo.append(teacherInfo[counter].get_text())

    # If allStatusList is empty, it means that there were no matching classes given the search criteria
    if not allStatusList:
        print("==========NO MATCH==========\n")
        input("Press Enter to continue...")     # Let the user press enter when they are ready for the program to terminate
        return 0                                # Exit the program with 0 exit code

    print("==========OPEN SECTIONS==========\n")    # Print the open sections first
    for counter in range(0, len(allStatusList)):    # Iterate through the classes found
        if allStatusList[counter] == 'Open':        # Take action for classes that are open. Print their info. 
            print(allClassCode[counter])
            print("\n")
            print(allTimeInfo[counter])
            print("\n")
            print(allTeacherInfo[counter])
            print("\n")
            print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n")

    print("==========CLOSED SECTIONS==========\n")  # Print closed sections second
    for counter in range(0, len(allStatusList)):    # Iterate through the classes found
        if allStatusList[counter] == 'Closed':      # Take action for classes that are closed. Print their info.
            print(allClassCode[counter])
            print("\n")
            print(allTimeInfo[counter])
            print("\n")
            print(allTeacherInfo[counter])
            print("\n")
            print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n")

    input("Press Enter to continue...")         # Let the user press enter when they are ready for the program to terminate
    return 0                                    # Exit the program with 0 exit code

#=================== PRE PROVIDED CODE. DO NOT TOUCH! ===================#

# Funtion to login
def login(user,pwd, programName, courseN):
    driver = webdriver.Chrome()
    driver.get(URL)
    LoginStat = False
    while True:
        time.sleep(0.05)
        if LOGIN_INI == driver.current_url:
           print("Opening...")
           break;
    try:
        loginForm = driver.find_element_by_xpath('//*[@id="userid"]')
        #longinForm.click()
        loginForm.send_keys(user)
        #time.sleep(1)
        print('Entering credentials')
        passwordForm = driver.find_element_by_xpath('//*[@id="pwd"]')
        passwordForm.click()
        passwordForm.send_keys(pwd)
        submit = driver.find_element_by_xpath('//*[@name="submit"]')
        submit.click()
        print(user + " login in process...")
    except:
        print(user + " input error")

    while True:
        time.sleep(0.05)
        if LOGIN_URL == driver.current_url :
           print(user + " login succeeded")
           break
               #time.sleep(5)
    searchCourse(driver, user, programName, courseN)

if __name__ == "__main__":
    # username & passwords
    data = ACCOUNTS
    # argument sys.argv[1] is discipline name, such as CSC, MAT, BIO, csc, bio ...
    # argument sys.argv[2] is course number, such as 220, 230, 250, 360
    
    # build threads
    threads = []
    for account, pwd in data.items():
        t = Thread(target=login,args=(account,pwd, sys.argv[1], sys.argv[2]))
        threads.append(t)
    for thr in threads:
        time.sleep(0.1)
        thr.start()

#=================== END OF PRE-PROVIDED CODE  ==========================#