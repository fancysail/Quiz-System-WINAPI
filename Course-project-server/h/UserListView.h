#pragma once
#include <string>
#include <sstream>
using namespace std;
enum Status{
	NOT_CONNECTED,
	CONNECTED,
	SUBMITTED
};
class UserListView {
private:
	string fullName;
	double percents;
	string date;
	Status status;
public:
	UserListView(string f, double per = 0) { fullName = f; percents = per; status = NOT_CONNECTED; }
	void setFullName(string s) { fullName = s; }
	void setPercents(double d) { percents = d; }
	void setDate(string d) { date = d; }
	void setStatus(Status st) { status = st; }

	Status getStatus() { return status; }
	string getFullName() { return fullName; }
	double getPercents() { return percents; }
	string getDate() { return date; }
};