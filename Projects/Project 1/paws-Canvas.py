from selenium import webdriver
import datetime
import time
import sys
from selenium.webdriver.common.keys import Keys
from threading import Thread
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from bs4 import BeautifulSoup
import re

#replace username/password with yours
#Do not share your credential with anyone!!!
ACCOUNTS = {
    "username":"password"
}

URL = 'https://paws.tcnj.edu/'
# login webpage
LOGIN_INI = 'https://paws.tcnj.edu/psp/paws/?cmd=login&languageCd=ENG&'
# after login webpage
# you need to change this URL. The current one is for faculty.
LOGIN_URL = 'https://paws.tcnj.edu/psp/paws/EMPLOYEE/SA/h/?tab=DEFAULT'
StartingTime = '2019-08-16 10:07:10'

def searchCourse(driver, user, programName, courseN):
    # you need to implement this function
    time.sleep(2)

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
        print('send username')
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
           print(user + " login succeed")
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
        time.sleep(0.05)
        thr.start()

