#include <Alert.h>
#include <Application.h>
#include <Button.h>
#include <LayoutBuilder.h>
#include <ListView.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <PopUpMenu.h>
#include <ScrollView.h>
#include <String.h>
#include <StringItem.h>
#include <StringView.h>
#include <TextControl.h>
#include <Window.h>

#include <stdio.h>

enum {
	MSG_ADD = 'addt',
	MSG_REMOVE = 'rmtk',
	MSG_CLEAR = 'clrt'
};

class TaskBoardWindow : public BWindow {
public:
	TaskBoardWindow()
		: BWindow(BRect(100, 100, 710, 520), "Haiku TaskBoard",
			B_TITLED_WINDOW, B_QUIT_ON_WINDOW_CLOSE)
	{
		fTaskInput = new BTextControl("task", "Cong viec:", "", NULL);

		BPopUpMenu* priorities = new BPopUpMenu("priorityMenu");
		priorities->AddItem(new BMenuItem("Cao", NULL));
		priorities->AddItem(new BMenuItem("Trung binh", NULL));
		priorities->AddItem(new BMenuItem("Thap", NULL));
		priorities->ItemAt(1)->SetMarked(true);
		fPriority = new BMenuField("priority", "Uu tien:", priorities);

		fTasks = new BListView("tasks", B_SINGLE_SELECTION_LIST);
		fTasks->AddItem(new BStringItem("[Cao] Hoan thanh bai tap lon"));
		fTasks->AddItem(new BStringItem("[Trung binh] Kiem tra Haiku API"));
		fTasks->AddItem(new BStringItem("[Thap] Chup anh ket qua"));

		fStatus = new BStringView("status", "Tasks: 3 | Haiku native app");
		BButton* add = new BButton("add", "Them task", new BMessage(MSG_ADD));
		BButton* remove = new BButton("remove", "Xoa task da chon",
			new BMessage(MSG_REMOVE));
		BButton* clear = new BButton("clear", "Xoa tat ca", new BMessage(MSG_CLEAR));

		BLayoutBuilder::Group<>(this, B_VERTICAL, 10)
			.SetInsets(14, 14, 14, 14)
			.Add(new BStringView("heading", "HAIKU TASKBOARD - Quan ly cong viec"))
			.Add(fTaskInput)
			.Add(fPriority)
			.AddGroup(B_HORIZONTAL, 8)
				.AddGlue()
				.Add(add)
			.End()
			.Add(new BScrollView("taskScroll", fTasks, B_FRAME_EVENTS, false, true), 1.0f)
			.AddGroup(B_HORIZONTAL, 8)
				.Add(fStatus)
				.AddGlue()
				.Add(clear)
				.Add(remove)
			.End();
	}

	void MessageReceived(BMessage* message) override
	{
		switch (message->what) {
			case MSG_ADD:
				AddTask();
				break;
			case MSG_REMOVE:
				RemoveSelectedTask();
				break;
			case MSG_CLEAR:
				ClearTasks();
				break;
			default:
				BWindow::MessageReceived(message);
		}
	}

private:
	void AddTask()
	{
		const char* task = fTaskInput->Text();
		if (task == NULL || task[0] == '\0') {
			(new BAlert("empty", "Hay nhap ten cong viec.", "OK"))->Go();
			return;
		}

		BMenuItem* selected = fPriority->Menu()->FindMarked();
		BString item("[");
		item << (selected != NULL ? selected->Label() : "Trung binh") << "] " << task;
		fTasks->AddItem(new BStringItem(item.String()));
		fTaskInput->SetText("");
		fTaskInput->MakeFocus(true);
		UpdateStatus();
	}

	void RemoveSelectedTask()
	{
		int32 index = fTasks->CurrentSelection();
		if (index < 0) {
			(new BAlert("select", "Chon mot task truoc khi xoa.", "OK"))->Go();
			return;
		}
		delete fTasks->RemoveItem(index);
		UpdateStatus();
	}

	void ClearTasks()
	{
		while (fTasks->CountItems() > 0)
			delete fTasks->RemoveItem((int32)0);
		UpdateStatus();
	}

	void UpdateStatus()
	{
		char text[80];
		snprintf(text, sizeof(text), "Tasks: %ld | Haiku native app",
			(long)fTasks->CountItems());
		fStatus->SetText(text);
	}

	BTextControl* fTaskInput;
	BMenuField* fPriority;
	BListView* fTasks;
	BStringView* fStatus;
};

class TaskBoardApp : public BApplication {
public:
	TaskBoardApp() : BApplication("application/x-vnd.btl-taskboard") {}

	void ReadyToRun() override
	{
		(new TaskBoardWindow())->Show();
	}
};

int main()
{
	TaskBoardApp app;
	app.Run();
	return 0;
}
