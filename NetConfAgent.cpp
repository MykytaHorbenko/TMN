#pragma once

#include "NetConfAgent.hpp"
#include "MobileClient.hpp"

using namespace std;
using namespace mobileclient;

namespace
{
    const int MAX_LEN = 100;

    /* Helper function for printing nodes. */
void print_node(libyang::S_Data_Node &node)
{
    libyang::S_Schema_Node schema = node->schema();

    //cout << nodetype2str(schema->nodetype()) << " \"" << schema->name() << '\"' << endl;
    cout << '\t' << "Path: " << node->path() << endl;
    //cout << '\t' << "Default: " << (node->dflt() ? "yes" : "no") << endl;

    /* type-specific print */
    switch (schema->nodetype()) {
    case LYS_CONTAINER:
    {
        // libyang::Schema_Node_Container scont(schema);

        // cout << '\t' << "Presence: " << (scont.presence() ? "yes" : "no") << endl;
        break;
    }
    case LYS_LEAF:
    {
        libyang::Data_Node_Leaf_List leaf(node);
        libyang::Schema_Node_Leaf sleaf(schema);

        cout << '\t' << "Value: \"" << leaf.value_str() << '\"' << endl;
        //cout << '\t' << "Is key: " << (sleaf.is_key() ? "yes" : "no") << endl;
        break;
    }
    case LYS_LEAFLIST:
    {
        libyang::Data_Node_Leaf_List leaflist(node);

        cout << '\t' << "Value: \"" << leaflist.value_str() << '\"' << endl;
        break;
    }
    case LYS_LIST:
    {
        // libyang::Schema_Node_List slist(schema);

        // cout << '\t' << "Keys:";
        // for (libyang::S_Schema_Node_Leaf &key : slist.keys()) {
        //     cout << ' ' << key->name();
        // }
        // cout << endl;
        break;
    }
    default:
        break;
    }

    cout << endl;
}

/* Helper function for writing notes to map. */
void write_node(libyang::S_Data_Node &node, map <string, string>& map)
{
    string path, value;
    libyang::S_Schema_Node schema = node->schema();

    //cout << nodetype2str(schema->nodetype()) << " \"" << schema->name() << '\"' << endl;
    //cout << '\t' << "Path: " << node->path() << endl;
    path = node->path();
    //cout << '\t' << "Default: " << (node->dflt() ? "yes" : "no") << endl;

    /* type-specific print */
    switch (schema->nodetype()) {
    case LYS_CONTAINER:
    {
        // libyang::Schema_Node_Container scont(schema);

        // cout << '\t' << "Presence: " << (scont.presence() ? "yes" : "no") << endl;
        break;
    }
    case LYS_LEAF:
    {
        libyang::Data_Node_Leaf_List leaf(node);
        libyang::Schema_Node_Leaf sleaf(schema);

        //cout << '\t' << "Value: \"" << leaf.value_str() << '\"' << endl;
        value = leaf.value_str();
        //cout << '\t' << "Is key: " << (sleaf.is_key() ? "yes" : "no") << endl;
        break;
    }
    case LYS_LEAFLIST:
    {
        libyang::Data_Node_Leaf_List leaflist(node);

        //cout << '\t' << "Value: \"" << leaflist.value_str() << '\"' << endl;
        value = leaflist.value_str();
        break;
    }
    case LYS_LIST:
    {
        // libyang::Schema_Node_List slist(schema);

        // cout << '\t' << "Keys:";
        // for (libyang::S_Schema_Node_Leaf &key : slist.keys()) {
        //     cout << ' ' << key->name();
        // }
        // cout << endl;
        break;
    }
    default:
        break;
    }
    map.emplace(path, value);

    //cout << endl;
}

/* Helper function for printing events. */
const char *ev_to_str(sr_event_t ev) {
    switch (ev) {
    case SR_EV_CHANGE:
        return "change";
    case SR_EV_DONE:
        return "done";
    case SR_EV_ABORT:
    default:
        return "abort";
    }
}

/* Helper function for writing changes given operation to map, old and new value. */
void print_change(sysrepo::S_Change change) 
{
    cout << endl;
    switch(change->oper()) {
    case SR_OP_CREATED:
        if (nullptr != change->new_val()) {
           cout <<"CREATED: ";
           cout << change->new_val()->to_string();
        }
        break;
    case SR_OP_DELETED:
        if (nullptr != change->old_val()) {
           cout << "DELETED: ";
           cout << change->old_val()->to_string();
        }
    break;
    case SR_OP_MODIFIED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) {
           cout << "MODIFIED: ";
           cout << "old value ";
           cout << change->old_val()->to_string();
           cout << "new value ";
           cout << change->new_val()->to_string();
        }
    break;
    case SR_OP_MOVED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " after ";
           cout << change->old_val()->xpath();
        } else if (nullptr != change->new_val()) {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " first";
        }
    break;
    }
}

/* Helper function for printing changes given operation, old and new value. */
void write_change(sysrepo::S_Change change, map <string, string>& map) 
{
    string path, value;
    cout << endl;
    switch(change->oper()) {
    case SR_OP_CREATED:
        if (nullptr != change->new_val()) {
            path = "inputNumber";
            value = change->new_val()->data()->get_string();
            //cout << change->new_val()->to_string();
            map.emplace(path, value);
        }
        break;
    case SR_OP_DELETED:
        if (nullptr != change->old_val()) {
           cout << "DELETED: ";
           cout << change->old_val()->to_string();
        }
    break;
    case SR_OP_MODIFIED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) {
           cout << "MODIFIED: ";
           cout << "old value ";
           cout << change->old_val()->to_string();
           cout << "new value ";
           cout << change->new_val()->to_string();
        }
    break;
    case SR_OP_MOVED:
        if (nullptr != change->old_val() && nullptr != change->new_val()) {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " after ";
           cout << change->old_val()->xpath();
        } else if (nullptr != change->new_val()) {
           cout << "MOVED: ";
           cout << change->new_val()->xpath();
           cout << " first";
        }
    break;
    }
}

/* Helper function for exiting app. */
volatile int exit_application = 0;
void sigint_handler(int signum)
{
    exit_application = 1;
}

/* Helper function for printing values. */
void print_value(sysrepo::S_Val value)
{
    cout << value->xpath();
    cout << " ";
    switch (value->type()) {
    case SR_CONTAINER_T:
    case SR_CONTAINER_PRESENCE_T:
        cout << "(container)" << endl;
        break;
    case SR_LIST_T:
        cout << "(list instance)" << endl;
        break;
    case SR_STRING_T:
        cout << "= " << value->data()->get_string() << endl;;
        break;
    case SR_BOOL_T:
    if (value->data()->get_bool())
            cout << "= true" << endl;
    else
            cout << "= false" << endl;
        break;
    case SR_ENUM_T:
        cout << "= " << value->data()->get_enum() << endl;;
        break;
    case SR_UINT8_T:
        cout << "= " << unsigned(value->data()->get_uint8()) << endl;
        break;
    case SR_UINT16_T:
        cout << "= " << unsigned(value->data()->get_uint16()) << endl;
        break;
    case SR_UINT32_T:
        cout << "= " << unsigned(value->data()->get_uint32()) << endl;
        break;
    case SR_UINT64_T:
        cout << "= " << unsigned(value->data()->get_uint64()) << endl;
        break;
    case SR_INT8_T:
        cout << "= " << value->data()->get_int8() << endl;
        break;
    case SR_INT16_T:
        cout << "= " << value->data()->get_int16() << endl;
        break;
    case SR_INT32_T:
        cout << "= " << value->data()->get_int32() << endl;
        break;
    case SR_INT64_T:
        cout << "= " << value->data()->get_int64() << endl;
        break;
     case SR_IDENTITYREF_T:
        cout << "= " << value->data()->get_identityref() << endl;
        break;
    case SR_BITS_T:
        cout << "= " << value->data()->get_bits() << endl;
        break;
    case SR_BINARY_T:
        cout << "= " << value->data()->get_binary() << endl;
        break;
    default:
        cout << "(unprintable)" << endl;
    }
    return;
}

}

namespace Netconfagent
{

bool NetConfAgent::initSysrepo() 
{
    Connection = std::make_shared<sysrepo::Connection>();
    Session = std::make_shared<sysrepo::Session>(Connection);
    Subscribe = std::make_shared<sysrepo::Subscribe>(Session);
    //std::cout<<"Session began"<<std::endl;
    return 1;
}

bool NetConfAgent::fetchData(string _xpath, map <string, string>& map)
{
    const char *xpath = _xpath.c_str();
    //sr_datastore_t ds = SR_DS_RUNNING;
    //const char *xpath = "/mobile-network:core/subscribers[number='001']";
    
    try {
        libyang::S_Data_Node data = Session->get_data(xpath);
        /* go through all top-level siblings */
        for (libyang::S_Data_Node &root : data->tree_for()) {
            /* go through all the children of a top-level sibling */
            for (libyang::S_Data_Node &node : root->tree_dfs()) {
                //print_node(node);
                write_node(node, map);
            }
        }
    } catch( const std::exception& e ) {
        cout << e.what() << endl;
    }

    return true;
}

bool NetConfAgent::subscribeForModelChanges(mobileclient::MobileClient& client, std::string& xpathForFetch)

{
    const char *module_name = "mobile-network";

        auto cb = [=, &client] (sysrepo::S_Session Session, const char *module_name, const char *xpath, sr_event_t event,
            uint32_t request_id) mutable {
            char change_path[MAX_LEN];
                map < string, string > mapFetchData;
                if(ev_to_str(event) == "done")
                {
                //mapFetchData.clear();
                //cout << "========== Notification came ========================="<<endl;
                fetchData(xpathForFetch, mapFetchData);

                //printing map
                // cout<<"Printing map"<<endl;
    	        // for (auto i : mapFetchData)
                // {
    	        // cout << i.first << " : " << i.second << endl;
    	        // }
                client.handleModuleChange(mapFetchData);
                }
                // cout << "\n\n ========== Notification " << ev_to_str(event) << " =============================================";

                // cout << "\n\n ========== CHANGES: =============================================\n" << endl;

                // snprintf(change_path, MAX_LEN, "/%s:*//.", module_name);

                // auto it = Session->get_changes_iter(change_path);
                
                // while (auto change = Session->get_change_next(it)) {
                //     print_change(change);
                //}

                // cout << "\n\n ========== END OF CHANGES =======================================\n" << endl;

            return SR_ERR_OK;

        };
        Subscribe->module_change_subscribe(module_name, cb);

    return true;
}

bool NetConfAgent::registerOperData(mobileclient::MobileClient& client, std::string xpathForSubscribe)
{
    //string name, xPath;
    //client.handleOperData(name, xPath);
    const char *module_name = "mobile-network";

    //string xpathForSubscribe = xPath.substr(0, xPath.find_last_of('/'));
    //const char *xpathForSubscribe;
    const char *xpathForSubscribePtr = xpathForSubscribe.c_str();

        auto cb2 = [=, &client] (sysrepo::S_Session Session, const char *module_name, const char *path, const char *request_xpath,
            uint32_t request_id, libyang::S_Data_Node &parent) {

            cout << "\n\n ========== CALLBACK CALLED TO PROVIDE \"" << path << "\" DATA ==========\n" << endl;
            string name, xPath;
            client.handleOperData(name, xPath);
            //xpathForSubscribe = path;

            libyang::S_Context ctx = Session->get_context();
            libyang::S_Module mod = ctx->get_module(module_name);

            parent->new_path(ctx, xPath.c_str(), name.c_str(), LYD_ANYDATA_CONSTSTRING, 0);

            // auto sub = std::make_shared<libyang::Data_Node>(parent, mod, "subscribers");
            // auto numb = std::make_shared<libyang::Data_Node>(sub, mod, "number", "911");
            // auto user_name = std::make_shared<libyang::Data_Node>(sub, mod, "userName", "Ivan");

            return SR_ERR_OK;
            
        };
        Subscribe->oper_get_items_subscribe(module_name, cb2, xpathForSubscribePtr);

}

bool NetConfAgent::changeData(string _xpath, string _newValue)
{
    const char *xpath = _xpath.c_str();
    const char *newValue = _newValue.c_str();

    try {
        auto value = std::make_shared<sysrepo::Val>(newValue);
        Session->set_item(xpath, value);

        Session->apply_changes();
    } catch( const std::exception& e ) {
        cout << e.what() << endl;
    }
}

bool NetConfAgent::subscribeForRpc(string _module_name, string _rpc_xpath)
{
    const char *module_name = _module_name.c_str();
    const char *rpc_xpath = _rpc_xpath.c_str();

    try {

        cout<<"Application will make an rpc call in "<< module_name<<endl;

        auto cbVals = [](sysrepo::S_Session session, const char* op_path, const sysrepo::S_Vals input, sr_event_t event, 
        uint32_t request_id, sysrepo::S_Vals_Holder output) 
        {
            cout << "\n ========== RPC CALLED ==========\n" << endl;

            auto out_vals = output->allocate(1);

            //for(size_t n = 0; n < input->val_cnt(); ++n)
                print_value(input->val(0));

            out_vals->val(0)->set("/mobile-network:delete/status",
                    "data deleted",
                    SR_STRING_T);

            return SR_ERR_OK;
        };

        cout << "\n ========== SUBSCRIBE TO RPC CALL ==========\n" << endl;

        Subscribe->rpc_subscribe(rpc_xpath, cbVals, 1);

        auto in_vals = std::make_shared<sysrepo::Vals>(1);

        in_vals->val(0)->set("/mobile-network:dlete/input-number",
                           "00000", SR_STRING_T);

        cout << "\n ========== START RPC CALL ==========\n" << endl;
        auto out_vals = Session->rpc_send(rpc_xpath, in_vals);

        cout << "\n ========== PRINT RETURN VALUE ==========\n" << endl;
        for(size_t n=0; n < out_vals->val_cnt(); ++n)
            print_value(out_vals->val(n));

        cout << "\n ========== END PROGRAM ==========\n" << endl;
    } catch( const std::exception& e ) {
        cout << e.what() << endl;
        return -1;
    }
}

bool NetConfAgent::notifySysrepo(string _module_name)
{
    const char *module_name = _module_name.c_str();
    try {

        cout<<"Application will send notification in "<<module_name<<endl;

        auto cbVals = [] (sysrepo::S_Session Session, const sr_ev_notif_type_t notif_type, const char *path,
            const sysrepo::S_Vals vals, time_t timestamp) {
            cout << "\n ========== NOTIF RECEIVED ==========\n" << endl;

            for(size_t n = 0; n < vals->val_cnt(); ++n) {
                print_value(vals->val(n));
            }
        };

        cout << "\n ========== SUBSCRIBE TO NOTIF ==========\n" << endl;

        Subscribe->event_notif_subscribe(module_name, cbVals);

        auto in_vals = std::make_shared<sysrepo::Vals>(2);

        in_vals->val(0)->set("/mobile-network:test-notif/val1", "some-value", SR_STRING_T);
        in_vals->val(1)->set("/mobile-network:test-notif/val2", "some-other-value", SR_STRING_T);

        cout << "\n ========== START NOTIF SEND ==========\n" << endl;
        Session->event_notif_send("/mobile-network:test-notif", in_vals);

        libyang::S_Context ctx = Connection->get_context();
        libyang::S_Module mod = ctx->get_module(module_name);
        auto in_trees = std::make_shared<libyang::Data_Node>(ctx, "/mobile-network:test-notif", nullptr, LYD_ANYDATA_CONSTSTRING, 0);
        std::make_shared<libyang::Data_Node>(libyang::Data_Node(in_trees, mod, "val1", "some-value"));
        std::make_shared<libyang::Data_Node>(libyang::Data_Node(in_trees, mod, "val2", "some-other-value"));

        cout << "\n ========== END PROGRAM ==========\n" << endl;
    } catch( const std::exception& e ) {
        cout << e.what() << endl;
        return -1;
    }
}

}