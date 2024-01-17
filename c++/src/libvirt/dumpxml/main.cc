#include <iostream>
#include <libvirt/libvirt.h>

int main()
{
    virConnectPtr conn = virConnectOpen(nullptr);
    virDomainPtr domain_ptr = virDomainLookupByName(conn, "cirros");

    char *xml_string = virDomainGetXMLDesc(domain_ptr, VIR_DOMAIN_XML_SECURE | VIR_DOMAIN_XML_INACTIVE);

    std::cout << xml_string << std::endl;

    delete xml_string;
    virDomainFree(domain_ptr);
    virConnectClose(conn);
}